#pragma once
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunneeded-internal-declaration"

#include "polycircuit/component.h"

#include "openfhe/pke/cryptocontext.h"
#include "openfhe/pke/scheme/ckksrns/ckksrns-utils.h"

namespace polycircuit {
namespace ckks {
template <typename ElementType>
class SHL final : public Component {
public:
    explicit SHL(lbcrypto::CryptoContext<ElementType>&& cc, lbcrypto::Ciphertext<ElementType>&& indexC,
                 lbcrypto::Ciphertext<ElementType>&& inputC)
        : m_cc(std::move(cc)), m_indexC(std::move(indexC)), m_inputC(std::move(inputC)) {}

    explicit SHL(const lbcrypto::CryptoContext<ElementType>& cc, const lbcrypto::Ciphertext<ElementType>& indexC,
                 lbcrypto::Ciphertext<ElementType>& inputC)
        : m_cc(cc), m_indexC(indexC), m_inputC(inputC) {}

    SHL(const SHL&) = default;

    SHL(SHL&&) = default;

    SHL& operator=(const SHL&) = default;

    SHL& operator=(SHL&&) = default;

    Ciphertext evaluate() override;

private:
    lbcrypto::CryptoContext<ElementType> m_cc;
    lbcrypto::Ciphertext<ElementType> m_indexC;
    lbcrypto::Ciphertext<ElementType> m_inputC;
};

namespace {

double Mod(const double a, const double q) {
    double result = std::fmod(a, q);
    if (result < 0) {
        result += q;
    }
    return result;
}

double ModInverse(const double a, const double q) {
    double t    = 0;
    double newT = 1;
    double r    = q;
    double newR = a;

    while (newR != 0) {
        const double quotient = std::floor(r / newR);
        std::swap(t, newT);
        newT -= quotient * t;
        std::swap(r, newR);
        newR -= quotient * r;
    }
    if (r > 1) {
        throw std::runtime_error("a is not invertible");
    }
    if (t < 0) {
        t += q;
    }
    return t;
}

std::shared_ptr<lbcrypto::longDiv> LongDivisionPolyModQ(const std::vector<double>& f, const std::vector<double>& g,
                                                        const double q = 786433)  // 786433 //65537
{
    uint32_t n       = lbcrypto::Degree(f);
    const uint32_t k = lbcrypto::Degree(g);

    if (n != f.size() - 1) {
        throw std::invalid_argument("LongDivisionPolyModQ: The dominant coefficient of the divident is zero.");
    }

    if (k != g.size() - 1) {
        throw std::invalid_argument("LongDivisionPolyModQ: The dominant coefficient of the divisor is zero.");
    }

    if (static_cast<int32_t>(n - k) < 0) {
        return std::make_shared<lbcrypto::longDiv>(std::vector<double>(1), f);
    }

    std::vector<double> qPoly(n - k + 1);
    std::vector<double> r(f);
    std::vector<double> d;
    d.reserve(g.size() + n);

    while (static_cast<int32_t>(n - k) >= 0) {
        d.clear();
        d.resize(n - k);
        d.insert(d.end(), g.begin(), g.end());

        qPoly[n - k] = Mod(r.back(), q);
        if (Mod(g[k], q) != 1) {
            qPoly[n - k] = Mod(qPoly[n - k] * ModInverse(g[k], q), q);
        }

        std::transform(d.begin(), d.end(), d.begin(),
                       [qPoly, n, k, q](const double coef) { return Mod(coef * qPoly[n - k], q); });
        std::transform(r.begin(), r.end(), d.begin(), r.begin(),
                       [q](const double coefR, const double coefD) { return Mod(coefR - coefD, q); });

        if (r.size() > 1) {
            n = lbcrypto::Degree(r);
            r.resize(n + 1);
        }
    }
    return std::make_shared<lbcrypto::longDiv>(qPoly, r);
}

template <typename ElementType>
lbcrypto::Ciphertext<ElementType> MultByInteger(const lbcrypto::Ciphertext<ElementType>& ct, const uint64_t scalar) {
    const std::vector<ElementType>& cv = ct->GetElements();

    std::vector<ElementType> resultDCRT(cv.size());
    for (usint i = 0; i < cv.size(); i++) {
        resultDCRT[i] = cv[i].Times(NativeInteger(scalar));
    }

    lbcrypto::Ciphertext<ElementType> result = ct->CloneZero();
    result->SetElements(std::move(resultDCRT));
    return result;
}

template <typename ElementType>
lbcrypto::Ciphertext<ElementType> EvalLinearWSum(const std::vector<lbcrypto::Ciphertext<ElementType>>& ctxs,
                                                 const std::vector<int64_t>& weights) {
    const auto cc = ctxs[0]->GetCryptoContext();

    lbcrypto::Ciphertext<ElementType> result = MultByInteger(ctxs[0], weights[0]);

    lbcrypto::Ciphertext<ElementType> tmp;
    for (uint32_t i = 1; i < weights.size(); i++) {
        tmp = MultByInteger(ctxs[i], weights[i]);
        cc->EvalAddInPlace(result, tmp);
    }

    return result;
}

template <typename ElementType>
void EvalAddInPlace(lbcrypto::Ciphertext<ElementType>& ct, const int64_t scalar) {
    const auto cc = ct->GetCryptoContext();
    cc->EvalAddInPlace(ct, cc->MakePackedPlaintext(std::vector<int64_t>(cc->GetRingDimension(), scalar)));
}

template <typename ElementType>
lbcrypto::Ciphertext<ElementType> EvalAdd(const lbcrypto::Ciphertext<ElementType>& ct, const int64_t scalar) {
    const auto cc = ct->GetCryptoContext();
    auto result   = ct->Clone();
    EvalAddInPlace(result, scalar);
    return result;
}

template <typename ElementType>
lbcrypto::Ciphertext<ElementType> InnerEvalPolyPS(lbcrypto::ConstCiphertext<ElementType> x,
                                                  const std::vector<double>& coefficients, uint32_t k, uint32_t m,
                                                  std::vector<lbcrypto::Ciphertext<ElementType>>& powers,
                                                  std::vector<lbcrypto::Ciphertext<ElementType>>& powers2) {
    const auto cc = x->GetCryptoContext();

    // Compute k*2^m because we use it often
    const uint32_t k2m2k = k * (1 << (m - 1)) - k;

    // Divide coefficients by x^{k*2^{m-1}}
    std::vector<double> xkm(int32_t(k2m2k + k) + 1, 0.0);
    xkm.back() = 1;

    const auto divqr = LongDivisionPolyModQ(coefficients, xkm);

    // Subtract x^{k(2^{m-1} - 1)} from r
    std::vector<double> r2 = divqr->r;
    if (static_cast<int32_t>(k2m2k - lbcrypto::Degree(divqr->r)) <= 0) {
        r2[static_cast<int32_t>(k2m2k)] -= 1;
        r2.resize(lbcrypto::Degree(r2) + 1);
    }
    else {
        r2.resize(static_cast<int32_t>(k2m2k + 1), 0.0);
        r2.back() = -1;
    }

    // Divide r2 by q
    const auto divcs = LongDivisionPolyModQ(r2, divqr->q);

    // Add x^{k(2^{m-1} - 1)} to s
    std::vector<double> s2 = divcs->r;
    s2.resize(static_cast<int32_t>(k2m2k + 1), 0.0);
    s2.back() = 1;

    lbcrypto::Ciphertext<ElementType> cu;
    const uint32_t dc = lbcrypto::Degree(divcs->q);
    bool flag_c       = false;

    if (dc >= 1) {
        if (dc == 1) {
            if (divcs->q[1] != 1) {
                cu = MultByInteger(powers.front(), static_cast<int64_t>(divcs->q[1]));
            }
            else {
                cu = powers.front()->Clone();
            }
        }
        else {
            std::vector<lbcrypto::Ciphertext<ElementType>> ctxs(dc);
            std::vector<int64_t> weights(dc);

            for (uint32_t i = 0; i < dc; i++) {
                ctxs[i]    = powers[i];
                weights[i] = static_cast<int64_t>(divcs->q[i + 1]);
            }

            cu = EvalLinearWSum(ctxs, weights);
        }

        // adds the free term (at x^0)
        EvalAddInPlace(cu, static_cast<int64_t>(divcs->q.front()));
        flag_c = true;
    }

    // Evaluate q and s2 at u. If their degrees are larger than k, then recursively apply the Paterson-Stockmeyer algorithm.
    lbcrypto::Ciphertext<ElementType> qu;

    if (lbcrypto::Degree(divqr->q) > k) {
        qu = InnerEvalPolyPS(x, divqr->q, k, m - 1, powers, powers2);
    }
    else {
        // dq = k from construction
        // perform scalar multiplication for all other terms and sum them up if there are non-zero coefficients
        auto qcopy = divqr->q;
        qcopy.resize(k);
        if (lbcrypto::Degree(qcopy) > 0) {
            std::vector<lbcrypto::Ciphertext<ElementType>> ctxs(lbcrypto::Degree(qcopy));
            std::vector<int64_t> weights(lbcrypto::Degree(qcopy));

            for (uint32_t i = 0; i < lbcrypto::Degree(qcopy); i++) {
                ctxs[i]    = powers[i];
                weights[i] = static_cast<int64_t>(divqr->q[i + 1]);
            }

            qu = EvalLinearWSum(ctxs, weights);
            // the highest order term will always be 1 because q is monic
            cc->EvalAddInPlace(qu, powers[k - 1]);
        }
        else {
            qu = powers[k - 1]->Clone();
        }
        // adds the free term (at x^0)
        EvalAddInPlace(qu, static_cast<int64_t>(divqr->q.front()));
    }

    const uint32_t ds = lbcrypto::Degree(s2);
    lbcrypto::Ciphertext<ElementType> su;

    if (std::equal(s2.begin(), s2.end(), divqr->q.begin())) {
        su = qu->Clone();
    }
    else if (ds > k) {
        su = InnerEvalPolyPS(x, s2, k, m - 1, powers, powers2);
    }
    else {
        // ds = k from construction
        // perform scalar multiplication for all other terms and sum them up if there are non-zero coefficients
        auto scopy = s2;
        scopy.resize(k);
        if (lbcrypto::Degree(scopy) > 0) {
            std::vector<lbcrypto::Ciphertext<ElementType>> ctxs(lbcrypto::Degree(scopy));
            std::vector<int64_t> weights(lbcrypto::Degree(scopy));

            for (uint32_t i = 0; i < lbcrypto::Degree(scopy); i++) {
                ctxs[i]    = powers[i];
                weights[i] = static_cast<int64_t>(s2[i + 1]);
            }

            su = EvalLinearWSum(ctxs, weights);
            // the highest order term will always be 1 because q is monic
            cc->EvalAddInPlace(su, powers[k - 1]);
        }
        else {
            su = powers[k - 1]->Clone();
        }
        // adds the free term (at x^0)
        EvalAddInPlace(su, static_cast<int64_t>(s2.front()));
    }

    lbcrypto::Ciphertext<ElementType> result;

    if (flag_c) {
        result = cc->EvalAdd(powers2[m - 1], cu);
    }
    else {
        result = EvalAdd(powers2[m - 1], static_cast<int64_t>(divcs->q.front()));
    }

    result = cc->EvalMult(result, qu);
    cc->EvalAddInPlace(result, su);

    return result;
}

template <typename ElementType>
lbcrypto::Ciphertext<ElementType> EvalPolyPS(lbcrypto::ConstCiphertext<ElementType> x,
                                             const std::vector<double>& coefficients) {
    const uint32_t n = lbcrypto::Degree(coefficients);

    std::vector<double> f2 = coefficients;

    // Make sure the coefficients do not have the dominant terms zero
    if (coefficients[coefficients.size() - 1] == 0) {
        f2.resize(n + 1);
    }

    std::vector<uint32_t> degs = lbcrypto::ComputeDegreesPS(n);
    const uint32_t k           = degs[0];
    const uint32_t m           = degs[1];

    // set the indices for the powers of x that need to be computed to 1
    std::vector<int32_t> indices(k, 0);
    for (size_t i = k; i > 0; i--) {
        if (!(i & (i - 1))) {
            // if i is a power of 2
            indices[i - 1] = 1;
        }
        else {
            // non-power of 2
            indices[i - 1]   = 1;
            int64_t powerOf2 = 1 << static_cast<int64_t>(std::floor(std::log2(i)));
            int64_t rem      = i % powerOf2;
            if (indices[rem - 1] == 0) {
                indices[rem - 1] = 1;
            }

            // while rem is not a power of 2 set indices required to compute rem to 1
            while (rem & (rem - 1)) {
                powerOf2 = 1 << static_cast<int64_t>(std::floor(std::log2(rem)));
                rem      = rem % powerOf2;
                if (indices[rem - 1] == 0) {
                    indices[rem - 1] = 1;
                }
            }
        }
    }

    std::vector<lbcrypto::Ciphertext<ElementType>> powers(k);
    powers[0] = x->Clone();
    auto cc   = x->GetCryptoContext();

    // computes all powers up to k for x
    for (size_t i = 2; i <= k; i++) {
        if (!(i & (i - 1))) {
            // if i is a power of two
            powers[i - 1] = cc->EvalSquare(powers[i / 2 - 1]);
        }
        else if (indices[i - 1] == 1) {
            // non-power of 2
            const int64_t powerOf2 = 1 << static_cast<int64_t>(std::floor(std::log2(i)));
            const int64_t rem      = i % powerOf2;
            powers[i - 1]          = cc->EvalMult(powers[powerOf2 - 1], powers[rem - 1]);
        }
    }

    const auto cryptoParams =
        std::dynamic_pointer_cast<lbcrypto::CryptoParametersCKKSRNS>(powers[k - 1]->GetCryptoParameters());

    std::vector<lbcrypto::Ciphertext<ElementType>> powers2(m);

    // computes powers of form k*2^i for x
    powers2.front() = powers.back()->Clone();
    for (uint32_t i = 1; i < m; i++) {
        powers2[i] = cc->EvalSquare(powers2[i - 1]);
    }

    // computes the product of the powers in power2, that yield x^{k(2*m - 1)}
    auto power2km1 = powers2.front()->Clone();
    for (uint32_t i = 1; i < m; i++) {
        power2km1 = cc->EvalMult(power2km1, powers2[i]);
    }

    // Compute k*2^{m-1}-k because we use it a lot
    const uint32_t k2m2k = k * (1 << (m - 1)) - k;

    // Add x^{k(2^m - 1)} to the polynomial that has to be evaluated
    f2.resize(2 * k2m2k + k + 1, 0.0);
    f2.back() = 1;

    // Divide f2 by x^{k*2^{m-1}}
    std::vector<double> xkm(static_cast<int32_t>(k2m2k + k) + 1, 0.0);
    xkm.back()       = 1;
    const auto divqr = LongDivisionPolyModQ(f2, xkm);

    // Subtract x^{k(2^{m-1} - 1)} from r
    std::vector<double> r2 = divqr->r;
    if (static_cast<int32_t>(k2m2k - lbcrypto::Degree(divqr->r)) <= 0) {
        r2[static_cast<int32_t>(k2m2k)] -= 1;
        r2.resize(lbcrypto::Degree(r2) + 1);
    }
    else {
        r2.resize(static_cast<int32_t>(k2m2k + 1), 0.0);
        r2.back() = -1;
    }

    // Divide r2 by q
    const auto divcs = LongDivisionPolyModQ(r2, divqr->q);

    // Add x^{k(2^{m-1} - 1)} to s
    std::vector<double> s2 = divcs->r;
    s2.resize(static_cast<int32_t>(k2m2k + 1), 0.0);
    s2.back() = 1;

    // Evaluate c at u
    lbcrypto::Ciphertext<ElementType> cu;
    const uint32_t dc = lbcrypto::Degree(divcs->q);
    bool flag_c       = false;

    if (dc >= 1) {
        if (dc == 1) {
            if (divcs->q[1] != 1) {
                cu = MultByInteger(powers.front(), static_cast<int64_t>(divcs->q[1]));
            }
            else {
                cu = powers.front()->Clone();
            }
        }
        else {
            std::vector<lbcrypto::Ciphertext<ElementType>> ctxs(dc);
            std::vector<int64_t> weights(dc);

            for (uint32_t i = 0; i < dc; i++) {
                ctxs[i]    = powers[i];
                weights[i] = static_cast<int64_t>(divcs->q[i + 1]);
            }

            cu = EvalLinearWSum(ctxs, weights);
        }

        // adds the free term (at x^0)
        EvalAddInPlace(cu, static_cast<int64_t>(divcs->q.front()));
        flag_c = true;
    }

    // Evaluate q and s2 at u. If their degrees are larger than k, then recursively apply the Paterson-Stockmeyer algorithm.
    lbcrypto::Ciphertext<ElementType> qu;

    if (lbcrypto::Degree(divqr->q) > k) {
        qu = InnerEvalPolyPS(x, divqr->q, k, m - 1, powers, powers2);
    }
    else {
        // dq = k from construction
        // perform scalar multiplication for all other terms and sum them up if there are non-zero coefficients
        auto qcopy = divqr->q;
        qcopy.resize(k);
        if (lbcrypto::Degree(qcopy) > 0) {
            std::vector<lbcrypto::Ciphertext<ElementType>> ctxs(lbcrypto::Degree(qcopy));
            std::vector<int64_t> weights(lbcrypto::Degree(qcopy));

            for (uint32_t i = 0; i < lbcrypto::Degree(qcopy); i++) {
                ctxs[i]    = powers[i];
                weights[i] = static_cast<int64_t>(divqr->q[i + 1]);
            }

            qu = EvalLinearWSum(ctxs, weights);
            // the highest order term will always be 1 because q is monic
            cc->EvalAddInPlace(qu, powers[k - 1]);
        }
        else {
            qu = powers[k - 1]->Clone();
        }
        // adds the free term (at x^0)
        EvalAddInPlace(qu, static_cast<int64_t>(divqr->q.front()));
    }

    const uint32_t ds = lbcrypto::Degree(s2);
    lbcrypto::Ciphertext<ElementType> su;

    if (std::equal(s2.begin(), s2.end(), divqr->q.begin())) {
        su = qu->Clone();
    }
    else if (ds > k) {
        su = InnerEvalPolyPS(x, s2, k, m - 1, powers, powers2);
    }
    else {
        // ds = k from construction
        // perform scalar multiplication for all other terms and sum them up if there are non-zero coefficients
        auto scopy = s2;
        scopy.resize(k);
        if (lbcrypto::Degree(scopy) > 0) {
            std::vector<lbcrypto::Ciphertext<ElementType>> ctxs(lbcrypto::Degree(scopy));
            std::vector<int64_t> weights(lbcrypto::Degree(scopy));

            for (uint32_t i = 0; i < lbcrypto::Degree(scopy); i++) {
                ctxs[i]    = powers[i];
                weights[i] = static_cast<int64_t>(s2[i + 1]);
            }

            su = EvalLinearWSum(ctxs, weights);
            // the highest order term will always be 1 because q is monic
            cc->EvalAddInPlace(su, powers[k - 1]);
        }
        else {
            su = powers[k - 1]->Clone();
        }
        // adds the free term (at x^0)
        EvalAddInPlace(su, static_cast<int64_t>(s2.front()));
    }

    lbcrypto::Ciphertext<ElementType> result;

    if (flag_c) {
        result = cc->EvalAdd(powers2[m - 1], cu);
    }
    else {
        result = cc->EvalAdd(powers2[m - 1], static_cast<int64_t>(divcs->q.front()));
    }

    result = cc->EvalMult(result, qu);

    cc->EvalAddInPlace(result, su);
    cc->EvalSubInPlace(result, power2km1);

    return result;
}

}  // unnamed namespace

template <typename ElementType>
Ciphertext SHL<ElementType>::evaluate() {
    int k = 1;
    while (k < 16) {
        m_indexC = m_cc->EvalAdd(m_indexC, m_cc->EvalRotate(m_indexC, -k));
        k *= 2;
    }

    std::vector<int64_t> v0(32768, 1);
    std::iota(std::begin(v0), std::begin(v0) + 16, 0);

    m_indexC = m_cc->EvalSub(m_indexC, m_cc->MakePackedPlaintext(v0));

    std::vector<double> coef = {1,      10802,  112723, 301260, 147254, 278067, 583275, 252765,
                                146881, 346114, 251411, 552443, 335648, 343498, 206133, 63890};

    m_indexC = m_cc->EvalSquare(m_indexC);
    m_indexC = EvalPolyPS<ElementType>(m_indexC, coef);

    k = 16;
    while (k < 32768) {
        m_indexC = m_cc->EvalAdd(m_indexC, m_cc->EvalRotate(m_indexC, -k));
        k *= 2;
    }

    std::vector<lbcrypto::Ciphertext<ElementType>> cS(15);
#pragma omp parallel for
    for (uint i = 0; i < 15; i++) {
        std::vector<int64_t> v(32768, 0);
        for (uint j = 0; j < 2048; j++) {
            v[i + 1 + j * 16] = 1;
        }

        cS[i] = m_cc->EvalMult(m_indexC, m_cc->MakePackedPlaintext(v));

        int k = 1;
        while (k < 16) {
            cS[i] = m_cc->EvalAdd(cS[i], m_cc->EvalRotate(cS[i], -k));
            k *= 2;
        }

        uint b = (i % 2) * 32768;
        uint M = 1 << (15 - i);
        uint h = i + 1;
        for (int64_t j = 0; j < 32768; j++) {
            v[j] = ((j + b) % M) << h;
        }

        cS[i] = m_cc->EvalMult(cS[i], m_cc->MakePackedPlaintext(v));
    }

    lbcrypto::Ciphertext<ElementType> cSAgg = cS[0]->Clone();
    for (uint i = 1; i < 15; i++) {
        cSAgg = m_cc->EvalAdd(cSAgg, cS[i]);
    }

    k = 1;
    while (k < 32768) {
        m_inputC = m_cc->EvalAdd(m_inputC, m_cc->EvalRotate(m_inputC, -k));
        k *= 2;
    }

    std::vector<lbcrypto::Ciphertext<ElementType>> c18(2);
    std::vector<lbcrypto::Ciphertext<ElementType>> c19(2);
    for (uint i = 0; i < 2; i++) {
        std::vector<int64_t> v(32768);
        std::iota(std::begin(v), std::end(v), 32768 * i);
        c18[i] = m_cc->EvalSub(m_inputC, m_cc->MakePackedPlaintext(v));

        lbcrypto::Ciphertext<ElementType> ct;
        for (uint j = 0; j < 18; j++) {
            c18[i] = m_cc->EvalSquare(c18[i]);
        }
        c19[i] = m_cc->EvalSquare(c18[i]);

        c18[i] = m_cc->EvalMult(c18[i], cSAgg);
        c19[i] = m_cc->EvalMult(c19[i], c18[i]);
    }

    lbcrypto::Ciphertext<ElementType> outputC = m_cc->EvalAdd(c19[0], c19[1]);

    outputC = m_cc->EvalNegate(outputC);
    outputC = m_cc->EvalAdd(outputC, cSAgg);
    outputC = m_cc->EvalAdd(outputC, cSAgg);

    k = 32768;
    while (k > 1) {
        k /= 2;
        outputC = m_cc->EvalAdd(outputC, m_cc->EvalRotate(outputC, -k));
    }
    outputC = m_cc->EvalRotate(outputC, -1);
    outputC = m_cc->EvalAdd(outputC, m_cc->EvalMult(m_inputC, m_indexC));

    return Ciphertext(std::move(outputC));
}
}  // namespace ckks
}  // namespace polycircuit
#pragma clang diagnostic pop