#pragma once

#include "polycircuit/component.h"

#include "openfhe/pke/cryptocontext.h"

namespace polycircuit {
    namespace ckks {
        template<typename ElementType>
        class lut final : public Component {
        public:
            explicit lut(lbcrypto::CryptoContext<ElementType> &&cc, lbcrypto::Ciphertext<ElementType> &&indexC,
                         lbcrypto::Ciphertext<ElementType> &&arrayC)
                    : m_cc(std::move(cc)), m_indexC(std::move(indexC)), m_arrayC(std::move(arrayC)) {}

            explicit lut(const lbcrypto::CryptoContext<ElementType> &cc,
                         const lbcrypto::Ciphertext<ElementType> &indexC,
                         lbcrypto::Ciphertext<ElementType> &arrayC)
                    : m_cc(cc), m_indexC(indexC), m_arrayC(arrayC) {}

            lut(const lut &) = default;

            lut(lut &&) = default;

            lut &operator=(const lut &) = default;

            lut &operator=(lut &&) = default;

            Ciphertext evaluate() override {
                lbcrypto::Ciphertext<ElementType> tempIndexCtx = m_cc->EvalRotate(m_indexC, -2047);

                int rotateIndex = 1;
                for (int i = 0; i < 11; i++) {
                    m_cc->EvalAddInPlace(tempIndexCtx, m_cc->EvalRotate(tempIndexCtx, rotateIndex));
                    rotateIndex <<= 1;
                }

                lbcrypto::Ciphertext<ElementType> eqCtx = evalEqual(std::move(tempIndexCtx));
                lbcrypto::Ciphertext<ElementType> tempResultCtx = m_cc->EvalMult(std::move(eqCtx), m_arrayC);

                rotateIndex = 1;
                for (int i = 0; i < 11; i++) {
                    m_cc->EvalAddInPlace(tempResultCtx, m_cc->EvalRotate(tempResultCtx, rotateIndex));
                    rotateIndex <<= 1;
                }
                return Ciphertext(std::move(tempResultCtx));
            }

        private:
            lbcrypto::Ciphertext<ElementType> evalEqual(lbcrypto::Ciphertext<ElementType> &&base) const {
                lbcrypto::Ciphertext<ElementType> result = m_cc->EvalSquare(m_cc->EvalAdd(std::move(base), seqPtxt()));

                for (int i = 0; i < 15; i++) {
                    m_cc->EvalSquareInPlace(result);
                }

                std::vector<int64_t> ones(2048, 1);
                lbcrypto::Plaintext onesPtxt = m_cc->MakePackedPlaintext(ones);

                return m_cc->EvalSub(std::move(onesPtxt), std::move(result));
            }

            lbcrypto::Plaintext seqPtxt() const {
                std::vector<int64_t> result;
                result.reserve(2048);
                for (int64_t i = 0; i < 2048; ++i) {
                    result.push_back(-i);
                }
                return m_cc->MakePackedPlaintext(result);
            }

        private:
            lbcrypto::CryptoContext<ElementType> m_cc;
            lbcrypto::Ciphertext<ElementType> m_indexC;
            lbcrypto::Ciphertext<ElementType> m_arrayC;
        };
    }  // namespace ckks
}  // namespace polycircuit
