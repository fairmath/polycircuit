#pragma once
#include <cmath>

#include "polycircuit/context.hpp"
#include "polycircuit/IComponent.hpp"

#include "openfhe/pke/cryptocontext.h"
#include "openfhe/pke/scheme/ckksrns/gen-cryptocontext-ckksrns-params.h"

namespace polycircuit
{
namespace algo 
{

namespace matrix 
{

CKKSParams mulDefaultParams() 
{
    lbcrypto::CCParams<lbcrypto::CryptoContextCKKSRNS> parameters;

    parameters.SetMultiplicativeDepth(2);
    parameters.SetScalingModSize(50);
    parameters.SetFirstModSize(55);
    parameters.SetScalingTechnique(lbcrypto::ScalingTechnique::FIXEDAUTO);
    parameters.SetBatchSize(4096);
    parameters.SetRingDim(65536);

    return {parameters, {1, -1, 2, -2, 3, -3, 4, -4, 5, -5}};
} 

class Mul final : public CKKSComponent
{
public:
    explicit Mul(CKKSCryptoContext&& context, CKKSPublicKey&& publicKey)
        : CKKSComponent(std::move(context), std::move(publicKey))
    { }

    explicit Mul(const CKKSCryptoContext& context, const CKKSPublicKey& publicKey)
        : CKKSComponent(context, publicKey)
    { }

    Mul(const Mul&) = default;
    Mul(Mul&&) = default;
    Mul& operator=(const Mul&) = default;
    Mul& operator=(Mul&&) = default;

private:
    CKKSCiphertext eval(CKKSCryptoContext cc, CKKSPublicKey publicKey, const CKKSArguments& args) override
    {
        int matrixSize = args.values().at(0);
        auto matrixAC = args.ciphertexts().at(0);
        auto matrixBC = args.ciphertexts().at(1);

        std::vector<double> mask1(2 * matrixSize * matrixSize, 0);
        std::vector<std::vector<double>> mask;
        mask.push_back(mask1);
        mask.push_back(std::move(mask1));

        std::vector<CKKSCiphertext> out(32);
        std::vector<lbcrypto::Plaintext> plaintext_mask(2);

        #pragma omp parallel for collapse(2)
        for (int k = 0; k < 4; k++)
        {
            for (int i = 0; i < matrixSize; i++)
            {
                const int t = (k % 2) * (i) + (1 - (k % 2)) * (i * matrixSize);
                mask[(k % 2)][t + (k / 2) * matrixSize * matrixSize] = 1;
            }
        }

        #pragma omp parallel for
        for (int j = 0; j < 2; j++)
        {
            plaintext_mask[j] = cc->MakeCKKSPackedPlaintext(mask[j]);
        }

        #pragma omp parallel sections
        {
            #pragma omp section
            matrixAC = cc->EvalAdd(matrixAC, cc->EvalRotate(matrixAC, -matrixSize * matrixSize + 1));
            #pragma omp section
            matrixBC = cc->EvalAdd(matrixBC, cc->EvalRotate(matrixBC, -matrixSize * matrixSize + matrixSize));
        }

        const int matrixSize_log = std::log2(matrixSize);
        const int matrixSizeHalf_log = std::log2(matrixSize / 2);

        #pragma omp parallel for shared(matrixAC, matrixBC, plaintext_mask)
        for (int t = 0; t < (matrixSize / 2); t++)
        {
            std::vector<CKKSCiphertext> ab1(2), ab2(2);
            if (t != 0)
            {
                ab1[0] = cc->EvalRotate(matrixAC, 2 * t);
                ab1[1] = cc->EvalRotate(matrixBC, 2 * t * matrixSize);
            }
            else
            {
                ab1[0] = matrixAC;
                ab1[1] = matrixBC;
            }
            for (int j = 0; j < 2; j++)
            {
                ab1[j] = cc->EvalMult(ab1[j], plaintext_mask[j]);
                for (int k = 0; k < matrixSize_log; k++)
                {
                    const int l = -1 * std::pow(2, k);
                    ab2[j] = cc->EvalRotate(ab1[j], l * std::pow(matrixSize, j));
                    ab1[j] = cc->EvalAdd(ab1[j], ab2[j]);
                }
            }
            out[t] = cc->EvalMult(ab1[0], ab1[1]);
        }

        for (int i = 1; i <= matrixSizeHalf_log; i++)
        {
            #pragma omp parallel for
            for (int t = 0; t < static_cast<int>(matrixSize / std::pow(2, i + 1)); t++)
            {
                cc->EvalAddInPlace(out[t], out[t + static_cast<int>(matrixSize / std::pow(2, i + 1))]);
            }
        }
        out[0] = cc->EvalAdd(out[0], cc->EvalRotate(out[0], matrixSize * matrixSize));

        return out[0];
    }
};

} // matrix
} // algo
} // polycircuit
