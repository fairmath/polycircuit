#pragma once

#include "polycircuit/component.h"

#include "openfhe/pke/cryptocontext.h"

namespace polycircuit {
namespace ckks {
template <typename ElementType>
class MatrixMul final : public Component {
public:
    explicit MatrixMul(const int matrixSize, lbcrypto::CryptoContext<ElementType>&& cc,
                       lbcrypto::Ciphertext<ElementType>&& matrixAC, lbcrypto::Ciphertext<ElementType>&& matrixBC)
        : m_matrixSize(matrixSize),
          m_cc(std::move(cc)),
          m_MatrixAC(std::move(matrixAC)),
          m_MatrixBC(std::move(matrixBC)) {}

    explicit MatrixMul(const int matrixSize, const lbcrypto::CryptoContext<ElementType>& cc,
                       const lbcrypto::Ciphertext<ElementType>& matrixAC,
                       const lbcrypto::Ciphertext<ElementType>& matrixBC)
        : m_matrixSize(matrixSize), m_cc(cc), m_MatrixAC(matrixAC), m_MatrixBC(matrixBC) {}

    MatrixMul(const MatrixMul&) = default;

    MatrixMul(MatrixMul&&) = default;

    MatrixMul& operator=(const MatrixMul&) = default;

    MatrixMul& operator=(MatrixMul&&) = default;

    Ciphertext evaluate() override {
        std::vector<double> mask1(2 * m_matrixSize * m_matrixSize, 0);
        std::vector<std::vector<double>> mask;
        mask.push_back(mask1);
        mask.push_back(std::move(mask1));

        std::vector<lbcrypto::Ciphertext<ElementType>> out(32);
        std::vector<lbcrypto::Plaintext> plaintext_mask(2);

#pragma omp parallel for collapse(2)
        for (int k = 0; k < 4; k++) {
            for (int i = 0; i < m_matrixSize; i++) {
                const int t = (k % 2) * (i) + (1 - (k % 2)) * (i * m_matrixSize);
                mask[(k % 2)][t + (k / 2) * m_matrixSize * m_matrixSize] = 1;
            }
        }

#pragma omp parallel for
        for (int j = 0; j < 2; j++) {
            plaintext_mask[j] = m_cc->MakeCKKSPackedPlaintext(mask[j]);
        }

#pragma omp parallel sections
        {
#pragma omp section
            m_MatrixAC = m_cc->EvalAdd(m_MatrixAC, m_cc->EvalRotate(m_MatrixAC, -m_matrixSize * m_matrixSize + 1));
#pragma omp section
            m_MatrixBC =
                m_cc->EvalAdd(m_MatrixBC, m_cc->EvalRotate(m_MatrixBC, -m_matrixSize * m_matrixSize + m_matrixSize));
        }

        const int m_matrixSize_log     = std::log2(m_matrixSize);
        const int m_matrixSizeHalf_log = std::log2(m_matrixSize / 2);

#pragma omp parallel for shared(m_MatrixAC, m_MatrixBC, plaintext_mask)
        for (int t = 0; t < (m_matrixSize / 2); t++) {
            std::vector<lbcrypto::Ciphertext<ElementType>> ab1(2), ab2(2);
            if (t != 0) {
                ab1[0] = m_cc->EvalRotate(m_MatrixAC, 2 * t);
                ab1[1] = m_cc->EvalRotate(m_MatrixBC, 2 * t * m_matrixSize);
            }
            else {
                ab1[0] = m_MatrixAC;
                ab1[1] = m_MatrixBC;
            }
            for (int j = 0; j < 2; j++) {
                ab1[j] = m_cc->EvalMult(ab1[j], plaintext_mask[j]);
                for (int k = 0; k < m_matrixSize_log; k++) {
                    const int l = -1 * std::pow(2, k);
                    ab2[j]      = m_cc->EvalRotate(ab1[j], l * std::pow(m_matrixSize, j));
                    ab1[j]      = m_cc->EvalAdd(ab1[j], ab2[j]);
                }
            }
            out[t] = m_cc->EvalMult(ab1[0], ab1[1]);
        }

        for (int i = 1; i <= m_matrixSizeHalf_log; i++) {
#pragma omp parallel for
            for (int t = 0; t < static_cast<int>(m_matrixSize / std::pow(2, i + 1)); t++) {
                m_cc->EvalAddInPlace(out[t], out[t + static_cast<int>(m_matrixSize / std::pow(2, i + 1))]);
            }
        }
        out[0] = m_cc->EvalAdd(out[0], m_cc->EvalRotate(out[0], m_matrixSize * m_matrixSize));
        return Ciphertext(std::move(out[0]));
    }

private:
    const int m_matrixSize;
    lbcrypto::CryptoContext<ElementType> m_cc;
    lbcrypto::Ciphertext<ElementType> m_MatrixAC;
    lbcrypto::Ciphertext<ElementType> m_MatrixBC;
};
}  // namespace ckks
}  // namespace polycircuit
