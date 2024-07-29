#pragma once

#include "polycircuit/IComponent.hpp"

#include "openfhe/pke/cryptocontext.h"

namespace polycircuit
{

template <typename ElementType>
class Relu final : public IComponent
{
public:
    explicit Relu(lbcrypto::CryptoContext<ElementType>&& cc,
        lbcrypto::Ciphertext<ElementType>&& inputC,
        lbcrypto::PublicKey<ElementType>&& publicKey)
        : m_cc(std::move(cc))
        , m_inputC(std::move(inputC))
        , m_publicKey(std::move(publicKey))
    { }
    explicit Relu(const lbcrypto::CryptoContext<ElementType>& cc,
        const lbcrypto::Ciphertext<ElementType>& inputC,
        const lbcrypto::PublicKey<ElementType>& publicKey)
        : m_cc(cc)
        , m_inputC(inputC)
        , m_publicKey(publicKey)
    { }
    Relu(const Relu&) = default;
    Relu(Relu&&) = default;
    Relu& operator=(const Relu&) = default;
    Relu& operator=(Relu&&) = default;

    Ciphertext evaluate() override
    {
        // Polynom of order 16 with leading integer coefficient

        // Declare parameters
        const std::vector<std::complex<double>> ai
        {
            0.0323949878919212, 0.500001412106499, 2.13483086933591, -4.78160418051218e-05,
            -13.9205486530553, 0.00061641818435605, 70.0957556465309, -0.00388040016141976,
            -213.087053403128, 0.0129145434432087, 385.924971250905, -0.0230082806472531,
            -407.029727261512, 0.0206280915579812, 230.348436664049, -0.00728306945833855
        };
        const int b = -54;
        std::vector<lbcrypto::Plaintext> aiplain(16);
        std::vector<lbcrypto::Ciphertext<ElementType>> aienc(16);

        // Use challenge ciphertext information and encrypt the parameters
        for (int k = 0; k < 16; k++)
        {
            std::vector<std::complex<double>> aik(16384);
            for (int m = 0; m < 16384; m++)
            {
                if (m == 1 || m == 12 || m == 18 || m == 41)
                {
                    if (m == 12 || m == 18)
                    {
                        aik[m] = 0;
                    }
                    else
                    {
                        if (k == 1)
                        {
                            aik[m] = 1;
                        }
                        else
                        {
                            aik[m] = 0;
                        }
                    }
                }
                else
                {
                    aik[m] = ai[k];
                }
            }
            aiplain[k] = m_cc->MakeCKKSPackedPlaintext(aik);
            aienc[k] = m_cc->Encrypt(m_publicKey, aiplain[k]);
        }

        // Build the monomials / coefficient combinations
        const auto x1 = m_inputC;
        const auto x2 = m_cc->EvalMult(x1, x1);
        const auto x3 = m_cc->EvalMult(x1, x2);
        const auto x4 = m_cc->EvalMult(x2, x2);
        const auto x5 = m_cc->EvalMult(x1, x4);
        const auto x6 = m_cc->EvalMult(x4, x2);
        const auto x7 = m_cc->EvalMult(x3, x4);
        const auto x8 = m_cc->EvalMult(x4, x4);

        const auto a1x1 = m_cc->EvalMult(aienc[1], x1);
        const auto a2x2 = m_cc->EvalMult(aienc[2], x2);
        const auto a3x3 = m_cc->EvalMult(aienc[3], x3);
        const auto a4x4 = m_cc->EvalMult(aienc[4], x4);
        const auto a5x5 = m_cc->EvalMult(aienc[5], x5);
        const auto a6x6 = m_cc->EvalMult(aienc[6], x6);
        const auto a7x7 = m_cc->EvalMult(aienc[7], x7);
        const auto a8x8 = m_cc->EvalMult(aienc[8], x8);

        const auto a9x1 = m_cc->EvalMult(aienc[9], x1);
        const auto a9x9 = m_cc->EvalMult(a9x1, x8);

        const auto a10x2 = m_cc->EvalMult(aienc[10], x2);
        const auto a10x10 = m_cc->EvalMult(a10x2, x8);

        const auto a11x3 = m_cc->EvalMult(aienc[11], x3);
        const auto a11x11 = m_cc->EvalMult(a11x3, x8);

        const auto a12x4 = m_cc->EvalMult(aienc[12], x4);
        const auto a12x12 = m_cc->EvalMult(a12x4, x8);

        const auto a13x1 = m_cc->EvalMult(aienc[13], x1);
        const auto a13x5 = m_cc->EvalMult(a13x1, x4);
        const auto a13x13 = m_cc->EvalMult(a13x5, x8);

        const auto a14x2 = m_cc->EvalMult(aienc[14], x2);
        const auto a14x6 = m_cc->EvalMult(a14x2, x4);
        const auto a14x14 = m_cc->EvalMult(a14x6, x8);

        const auto a15x1 = m_cc->EvalMult(aienc[15], x1);
        const auto a15x3 = m_cc->EvalMult(a15x1, x2);
        const auto a15x7 = m_cc->EvalMult(a15x3, x4);
        const auto a15x15 = m_cc->EvalMult(a15x7, x8);

        // Build polynomial from its parts
        lbcrypto::Ciphertext<ElementType> outputC = aienc[0];
        outputC = m_cc->EvalAdd(outputC, a1x1);
        outputC = m_cc->EvalAdd(outputC, a2x2);
        outputC = m_cc->EvalAdd(outputC, a3x3);
        outputC = m_cc->EvalAdd(outputC, a4x4);
        outputC = m_cc->EvalAdd(outputC, a5x5);
        outputC = m_cc->EvalAdd(outputC, a6x6);
        outputC = m_cc->EvalAdd(outputC, a7x7);
        outputC = m_cc->EvalAdd(outputC, a8x8);
        outputC = m_cc->EvalAdd(outputC, a9x9);
        outputC = m_cc->EvalAdd(outputC, a10x10);
        outputC = m_cc->EvalAdd(outputC, a11x11);
        outputC = m_cc->EvalAdd(outputC, a12x12);
        outputC = m_cc->EvalAdd(outputC, a13x13);
        outputC = m_cc->EvalAdd(outputC, a14x14);
        outputC = m_cc->EvalAdd(outputC, a15x15);

        // Leading integer coefficient by adding
        const auto x16 = m_cc->EvalMult(x8, x8);
        if (b >= 0)
        {
            for (int m = 0; m < b; m++)
            {
                outputC = m_cc->EvalAdd(outputC, x16);
            }
        }
        else
        {
            for (int m = 0; m < -b; m++)
            {
                outputC = m_cc->EvalSub(outputC, x16);
            }
        }
        return Ciphertext(std::move(outputC));
    }

private:
    lbcrypto::CryptoContext<ElementType> m_cc;
    lbcrypto::Ciphertext<ElementType> m_inputC;
    lbcrypto::PublicKey<ElementType> m_publicKey;
};

} // polycircuit
