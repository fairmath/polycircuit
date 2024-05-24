#include "openfhe.h"
#include <cmath>
#include "ciphertext-ser.h"
#include "cryptocontext-ser.h"
#include "key/key-ser.h"
#include "scheme/ckksrns/ckksrns-ser.h"
#include "scheme/scheme-id.h"

using namespace lbcrypto;

namespace polycircuit {
    template<class ComponentType, typename ElementType>
    typename ComponentType::result_type evaluate(
        CryptoContext<ElementType> m_cc,
        PublicKey<ElementType> m_PublicKey,
        typename ComponentType::params_type params);

    // A component for two square matrices multiplication
    template <std::uint32_t MatrixSize, SCHEME schemeId=CKKSRNS_SCHEME, class ElementType=DCRTPoly>
    struct matrix_mult {
        constexpr static const std::uint32_t n = MatrixSize;
        constexpr static const std::uint32_t n_log = std::log2(n);

        struct params_type{
            Ciphertext<ElementType> & m_MatrixAC;
            Ciphertext<ElementType> & m_MatrixBC;
        };

        using result_type = Ciphertext<ElementType>;
    };

    /*
     * Implementation of the matrix multiplication component evaluate function
     * For the CKKSRNS scheme and matrix size of 64.
     * This function changes its params.
     */
    template <class ElementType>
    typename matrix_mult<64, CKKSRNS_SCHEME, ElementType>::result_type evaluate(
            CryptoContext<ElementType> m_cc,
            PublicKey<ElementType> m_PublicKey,
            typename matrix_mult<64, CKKSRNS_SCHEME, ElementType>::params_type params){
        using component_type = matrix_mult<64, CKKSRNS_SCHEME, ElementType>;
        using ciphertext_type = Ciphertext<ElementType>;
        constexpr static const std::uint32_t n = component_type::n;

        Ciphertext<ElementType> &m_MatrixAC = params.m_MatrixAC;
        Ciphertext<ElementType> &m_MatrixBC = params.m_MatrixBC;

        std::vector<double> mask1(2 * n * n, 0);
        std::vector<std::vector<double>> mask;
        mask.push_back(mask1);
        mask.push_back(mask1);

        std::vector<ciphertext_type> out(32);
        std::vector<Plaintext> plaintext_mask(2);

#pragma omp parallel for collapse(2)
        for (int k = 0; k < 4; k++) {
            for (int i = 0; i < n; i++) {
                int t = (k % 2) * (i) + (1 - (k % 2)) * (i * n);
                mask[(k % 2)][t + (int)(k / 2) * n * n] = 1;
            }
        }

#pragma omp parallel for
        for (int j = 0; j < 2; j++) {
            plaintext_mask[j] = m_cc->MakeCKKSPackedPlaintext(mask[j]);
        }

#pragma omp parallel sections
        {
#pragma omp section
            m_MatrixAC = m_cc->EvalAdd(m_MatrixAC, m_cc->EvalRotate(m_MatrixAC, -n * n + 1));
#pragma omp section
            m_MatrixBC = m_cc->EvalAdd(m_MatrixBC, m_cc->EvalRotate(m_MatrixBC, -n * n + n));
        }

#pragma omp parallel for shared(m_MatrixAC, m_MatrixBC, plaintext_mask)
        for (int t = 0; t < (int)(n / 2); t++) {
            std::vector<ciphertext_type> ab1(2), ab2(2);
            if (t != 0) {

                ab1[0] = m_cc->EvalRotate(m_MatrixAC, 2 * t);
                ab1[1] = m_cc->EvalRotate(m_MatrixBC, 2 * t * n);

            } else {
                ab1[0] = m_MatrixAC;
                ab1[1] = m_MatrixBC;
            }

            for (int j = 0; j < 2; j++) {
                ab1[j] = m_cc->EvalMult(ab1[j], plaintext_mask[j]);
                for (int k = 0; k < component_type::n_log; k++) {
                    int l = -1 * pow(2, k);
                    ab2[j] = m_cc->EvalRotate(ab1[j], l * pow(n, j));
                    ab1[j] = m_cc->EvalAdd(ab1[j], ab2[j]);
                }
            }

            out[t] = m_cc->EvalMult(ab1[0], ab1[1]);
        }

        for (int i = 1; i <= (int)(log2(n / 2)); i++) {
#pragma omp parallel for
            for (int t = 0; t < (int)(n / pow(2, i + 1)); t++) {
                m_cc->EvalAddInPlace(out[t], out[t + (int)(n / pow(2, i + 1))]);
            }
        }

        out[0] = m_cc->EvalAdd(out[0], m_cc->EvalRotate(out[0], n * n));
        return out[0];
    }


} // namespace polycircuit