#pragma once

#include "polycircuit/context.hpp"
#include "polycircuit/IComponent.hpp"

#include "openfhe/pke/cryptocontext.h"
#include "openfhe/pke/scheme/ckksrns/gen-cryptocontext-ckksrns-params.h"

namespace polycircuit
{

namespace func 
{

CKKSParams reluDefaultParams() {
    lbcrypto::CCParams<lbcrypto::CryptoContextCKKSRNS> parameters;

    parameters.SetMultiplicativeDepth(4);
    parameters.SetScalingModSize(48);
    parameters.SetFirstModSize(55);
    parameters.SetScalingTechnique(lbcrypto::ScalingTechnique::FIXEDAUTO);
    parameters.SetBatchSize(16384);
    parameters.SetRingDim(32768);

    return {parameters, {}};
}    

class ReLU final : public CKKSComponent
{
public:
    explicit ReLU(CKKSCryptoContext&& context, CKKSPublicKey&& publicKey)
        : CKKSComponent(std::move(context), std::move(publicKey))
    { }

    explicit ReLU(const CKKSCryptoContext& context, const CKKSPublicKey& publicKey)
        : CKKSComponent(context, publicKey)
    { }

    ReLU(const ReLU&) = default;
    ReLU(ReLU&&) = default;
    ReLU& operator=(const ReLU&) = default;
    ReLU& operator=(ReLU&&) = default;

private:
    CKKSCiphertext eval(CKKSCryptoContext cc, CKKSPublicKey publicKey, const CKKSArguments& args) override
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
        std::vector<CKKSCiphertext> aienc(16);

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

            aiplain[k] = cc->MakeCKKSPackedPlaintext(aik);
            aienc[k] = cc->Encrypt(publicKey, aiplain[k]);
        }

        // Build the monomials / coefficient combinations
        const auto x1 = args.ciphertexts().at(0);
        const auto x2 = cc->EvalMult(x1, x1);
        const auto x3 = cc->EvalMult(x1, x2);
        const auto x4 = cc->EvalMult(x2, x2);
        const auto x5 = cc->EvalMult(x1, x4);
        const auto x6 = cc->EvalMult(x4, x2);
        const auto x7 = cc->EvalMult(x3, x4);
        const auto x8 = cc->EvalMult(x4, x4);

        const auto a1x1 = cc->EvalMult(aienc[1], x1);
        const auto a2x2 = cc->EvalMult(aienc[2], x2);
        const auto a3x3 = cc->EvalMult(aienc[3], x3);
        const auto a4x4 = cc->EvalMult(aienc[4], x4);
        const auto a5x5 = cc->EvalMult(aienc[5], x5);
        const auto a6x6 = cc->EvalMult(aienc[6], x6);
        const auto a7x7 = cc->EvalMult(aienc[7], x7);
        const auto a8x8 = cc->EvalMult(aienc[8], x8);

        const auto a9x1 = cc->EvalMult(aienc[9], x1);
        const auto a9x9 = cc->EvalMult(a9x1, x8);

        const auto a10x2 = cc->EvalMult(aienc[10], x2);
        const auto a10x10 = cc->EvalMult(a10x2, x8);

        const auto a11x3 = cc->EvalMult(aienc[11], x3);
        const auto a11x11 = cc->EvalMult(a11x3, x8);

        const auto a12x4 = cc->EvalMult(aienc[12], x4);
        const auto a12x12 = cc->EvalMult(a12x4, x8);

        const auto a13x1 = cc->EvalMult(aienc[13], x1);
        const auto a13x5 = cc->EvalMult(a13x1, x4);
        const auto a13x13 = cc->EvalMult(a13x5, x8);

        const auto a14x2 = cc->EvalMult(aienc[14], x2);
        const auto a14x6 = cc->EvalMult(a14x2, x4);
        const auto a14x14 = cc->EvalMult(a14x6, x8);

        const auto a15x1 = cc->EvalMult(aienc[15], x1);
        const auto a15x3 = cc->EvalMult(a15x1, x2);
        const auto a15x7 = cc->EvalMult(a15x3, x4);
        const auto a15x15 = cc->EvalMult(a15x7, x8);

        // Build polynomial from its parts
        CKKSCiphertext outputC = aienc[0];
        outputC = cc->EvalAdd(outputC, a1x1);
        outputC = cc->EvalAdd(outputC, a2x2);
        outputC = cc->EvalAdd(outputC, a3x3);
        outputC = cc->EvalAdd(outputC, a4x4);
        outputC = cc->EvalAdd(outputC, a5x5);
        outputC = cc->EvalAdd(outputC, a6x6);
        outputC = cc->EvalAdd(outputC, a7x7);
        outputC = cc->EvalAdd(outputC, a8x8);
        outputC = cc->EvalAdd(outputC, a9x9);
        outputC = cc->EvalAdd(outputC, a10x10);
        outputC = cc->EvalAdd(outputC, a11x11);
        outputC = cc->EvalAdd(outputC, a12x12);
        outputC = cc->EvalAdd(outputC, a13x13);
        outputC = cc->EvalAdd(outputC, a14x14);
        outputC = cc->EvalAdd(outputC, a15x15);

        // Leading integer coefficient by adding
        const auto x16 = cc->EvalMult(x8, x8);
        if (b >= 0)
        {
            for (int m = 0; m < b; m++)
            {
                outputC = cc->EvalAdd(outputC, x16);
            }
        }
        else
        {
            for (int m = 0; m < -b; m++)
            {
                outputC = cc->EvalSub(outputC, x16);
            }
        }
        return outputC;
    }
};

} // func
} // polycircuit
