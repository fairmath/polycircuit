#pragma once

#include "polycircuit/component.h"

#include "openfhe/pke/cryptocontext.h"

namespace polycircuit {
namespace ckks {
template <typename ElementType>
class matrxi_mul final : public Component {
public:
    explicit matrxi_mul(lbcrypto::CryptoContext<ElementType>&& cc, lbcrypto::Ciphertext<ElementType>&& arrayC,
                        std::vector<std::vector<int64_t>>&& coeffs, const size_t arraySize, const int infimum)
        : m_cc(std::move(cc)),
          m_arrayC(std::move(arrayC)),
          m_coeffs(std::move(coeffs)),
          m_arraySize(arraySize),
          m_infimum(infimum) {}

    explicit matrxi_mul(const lbcrypto::CryptoContext<ElementType>& cc, const lbcrypto::Ciphertext<ElementType>& arrayC,
                        const std::vector<std::vector<int64_t>>& coeffs, const size_t arraySize, const int infimum)
        : m_cc(cc), m_arrayC(arrayC), m_coeffs(coeffs), m_arraySize(arraySize), m_infimum(infimum) {}

    matrxi_mul(const matrxi_mul&) = default;

    matrxi_mul(matrxi_mul&&) = default;

    matrxi_mul& operator=(const matrxi_mul&) = default;

    matrxi_mul& operator=(matrxi_mul&&) = default;

    Ciphertext evaluate() override;

private:
    lbcrypto::CryptoContext<ElementType> m_cc;
    lbcrypto::Ciphertext<ElementType> m_arrayC;
    std::vector<std::vector<int64_t>> m_coeffs;
    size_t m_arraySize;  // 2048
    int m_infimum;       // 256
};

namespace {

template <typename ElementType>
lbcrypto::Ciphertext<ElementType> duplicateCiphertext(const lbcrypto::Ciphertext<ElementType>& x, size_t arraySize) {
    lbcrypto::CryptoContext<ElementType> cc = x->GetCryptoContext();
    lbcrypto::Ciphertext<ElementType> y     = x->Clone();
    const size_t batchSize                  = cc->GetRingDimension() / 2;
    while (arraySize < batchSize) {
        y = cc->EvalAdd(y, cc->EvalRotate(y, arraySize));
        arraySize <<= 1;
    }
    return y;
}

// Function to precompute the powers x^1, x^2, ..., up to x^degree.
template <typename ElementType>
void precomputePowersOfX(const lbcrypto::Ciphertext<ElementType>& x,
                         std::vector<lbcrypto::Ciphertext<ElementType>>& xPowers, const size_t degree) {
    lbcrypto::CryptoContext<ElementType> cc = x->GetCryptoContext();

    // Precompute powers of x using repeated squaring.
    xPowers.resize(degree + 1);
    xPowers[0] = nullptr;
    xPowers[1] = x;

    // Compute powers in stages to avoid race conditions.
    for (size_t step = 1; step <= degree; step *= 2) {
#pragma omp parallel for
        for (size_t i = step + 1; i <= std::min(2 * step, degree); ++i) {
            if (i % 2 == 0) {
                xPowers[i] = cc->EvalSquare(xPowers[i / 2]);
            }
            else {
                const size_t closestPowerOfTwo = step;
                xPowers[i] = cc->EvalMult(xPowers[closestPowerOfTwo], xPowers[i - closestPowerOfTwo]);
            }
        }
    }
}

// Collect the x^d monomial coefficients for multiple output polynomials.
std::vector<int64_t> getCoefficientSlice(const std::vector<std::vector<int64_t>>& coeffs, const size_t numElts,
                                         const size_t colIndex, const size_t initialRowIdx) {
    std::vector<int64_t> result(numElts, 0);
    for (size_t k = 0; k < numElts && initialRowIdx + k < coeffs.size(); k++) {
        result[k] = coeffs[initialRowIdx + k][colIndex];
    }
    return result;
}

std::vector<int64_t> repeatVector(std::vector<int64_t>& input, const int m) {
    const int n = input.size();
    std::vector<int64_t> result(m);

    // Calculate the repeat factor.
    const int repeatFactor = m / n;

    // Repeating the vector using std::fill_n.
    for (int i = 0; i < n; ++i) {
        std::fill(result.begin() + i * repeatFactor, result.begin() + (i + 1) * repeatFactor, input[i]);
    }
    return result;
}

template <typename ElementType>
lbcrypto::Ciphertext<ElementType> EvalMultVectorDensity(const lbcrypto::Ciphertext<ElementType>& ciphertext,
                                                        std::vector<int64_t>& vals) {
    lbcrypto::CryptoContext<ElementType> cc = ciphertext->GetCryptoContext();
    const size_t batchSize                  = cc->GetRingDimension() / 2;
    const auto v                            = repeatVector(vals, batchSize);
    const auto plaintext                    = cc->MakePackedPlaintext(v);
    return cc->EvalMult(ciphertext, plaintext);
}

template <typename ElementType>
lbcrypto::Ciphertext<ElementType> EvalAddVectorDensity(const lbcrypto::Ciphertext<ElementType>& ciphertext,
                                                       std::vector<int64_t>& vals) {
    lbcrypto::CryptoContext<ElementType> cc = ciphertext->GetCryptoContext();
    const size_t batchSize                  = cc->GetRingDimension() / 2;
    const auto v                            = repeatVector(vals, batchSize);
    const auto plaintext                    = cc->MakePackedPlaintext(v);
    return cc->EvalAdd(ciphertext, plaintext);
}

// Evaluate multiple polynomials over the same input ciphertext using the
// Density packing, e.g. [1 1 1 1 2 2 2 2 3 3 3 3 4 4 4 4].
template <typename ElementType>
void EvalMultiplePolyIntegersDensity(const lbcrypto::Ciphertext<ElementType>& x,
                                     const std::vector<std::vector<int64_t>>& coeffs,
                                     std::vector<lbcrypto::Ciphertext<ElementType>>& results, const size_t arraySize) {
    lbcrypto::CryptoContext<ElementType> cc = x->GetCryptoContext();
    const size_t batchSize                  = cc->GetRingDimension() / 2;
    const int density                       = batchSize / arraySize;
    const size_t degree                     = coeffs[0].size();
    const int numResults                    = (degree + density - 1) / density;

    std::vector<lbcrypto::Ciphertext<ElementType>> xPowers;
    precomputePowersOfX(x, xPowers, degree);
    results.resize(numResults);

#pragma omp parallel for
    for (int j = 0; j < numResults; j++) {
        auto c      = getCoefficientSlice(coeffs, density, 1, j * density);
        auto result = EvalMultVectorDensity(xPowers[1], c);

        // Add the constant term.
        c      = getCoefficientSlice(coeffs, density, 0, j * density);
        result = EvalAddVectorDensity(result, c);

        // Combine the terms.
        for (size_t i = 2; i < degree; ++i) {
            auto c    = getCoefficientSlice(coeffs, density, i, j * density);
            auto term = EvalMultVectorDensity(xPowers[i], c);
            result    = cc->EvalAdd(result, term);
        }
        results[j] = result;
    }
}

template <typename ElementType>
lbcrypto::Ciphertext<ElementType> EvalNandDensity(lbcrypto::Ciphertext<ElementType> x, const size_t arraySize) {
    lbcrypto::CryptoContext<ElementType> cc = x->GetCryptoContext();
    size_t r                                = 1;
    while (r < arraySize) {
        x = cc->EvalMult(x, cc->EvalRotate(x, r));
        r *= 2;
    }

    return x;
}

// Adds together all elements of results vector using reduce-fold parallel
// processing. Note: this modifies results in-place.
template <typename ElementType>
lbcrypto::Ciphertext<ElementType> parallelAggregate(std::vector<lbcrypto::Ciphertext<ElementType>>& results) {
    size_t numResults = results.size();
    if (numResults == 0) {
        throw std::invalid_argument("The results vector is empty.");
    }

    auto cc = results[0]->GetCryptoContext();

    while (numResults > 1) {
        size_t halfSize = numResults / 2;
#pragma omp parallel for
        for (size_t i = 0; i < halfSize; ++i) {
            cc->EvalAddInPlace(results[i], results[i + halfSize]);
        }
        if (numResults % 2 == 1) {
            results[halfSize] = results[numResults - 1];
            ++halfSize;
        }
        numResults = halfSize;
    }

    return results[0];
}

template <typename ElementType>
lbcrypto::Ciphertext<ElementType> EvalScalarAdd(const lbcrypto::Ciphertext<ElementType>& ciphertext,
                                                const int64_t val) {
    lbcrypto::CryptoContext<ElementType> cc = ciphertext->GetCryptoContext();
    const size_t size                       = cc->GetRingDimension() / 2;
    const std::vector<int64_t> vals(size, val);
    const auto plaintext = cc->MakePackedPlaintext(vals);
    return cc->EvalAdd(ciphertext, plaintext);
}

}  // unnamed namespace

// For all possible threshold values:
//     Evaluate step function to determine if each element is above threshold;
//     Compute boolean Nand circuit to determine if any element passed the
//     criteria.
// Sum result across all thresholds.
//
// Modified to pack multiple degrees into a single ciphertext
// because batchSize > array_size
template <typename ElementType>
Ciphertext matrxi_mul<ElementType>::evaluate() {
    const lbcrypto::Ciphertext<ElementType> arrayCduplicate = duplicateCiphertext(m_arrayC, m_arraySize);
    std::vector<lbcrypto::Ciphertext<ElementType>> results;
    EvalMultiplePolyIntegersDensity(arrayCduplicate, m_coeffs, results, m_arraySize);

    const size_t numResults = results.size();

#pragma omp parallel for
    for (size_t i = 0; i < numResults; i++) {
        results[i] = EvalNandDensity(results[i], m_arraySize);
        // off-label use of duplicateCiphertext to accumulate the outer Elements
        results[i] = duplicateCiphertext(results[i], m_arraySize);
    }

    lbcrypto::Ciphertext<ElementType> outputC = parallelAggregate(results);
    m_cc->EvalNegateInPlace(outputC);
    outputC = EvalScalarAdd(outputC, m_infimum - 1);

    return Ciphertext(std::move(outputC));
}
}  // namespace ckks
}  // namespace polycircuit
