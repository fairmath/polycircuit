#pragma once

#include "polycircuit/context.hpp"
#include "polycircuit/IComponent.hpp"

#include "openfhe/pke/cryptocontext.h"
#include "openfhe/pke/scheme/ckksrns/gen-cryptocontext-ckksrns-params.h"

namespace polycircuit
{
namespace algo 
{

CKKSParams lookupDefaultParams()
{
    lbcrypto::CCParams<lbcrypto::CryptoContextCKKSRNS> parameters;
    parameters.SetMultiplicativeDepth(17);
    parameters.SetBatchSize(2048);
    parameters.SetScalingModSize(55);
    parameters.SetFirstModSize(60);
    parameters.SetScalingTechnique(lbcrypto::ScalingTechnique::FLEXIBLEAUTO);
    parameters.SetRingDim(131072);

    return {parameters, {1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, -2047}};
}

class LookupTable final : public CKKSComponent
{
public:
    explicit LookupTable(CKKSCryptoContext&& context, CKKSPublicKey&& publicKey)
        : CKKSComponent(std::move(context), std::move(publicKey))
    { }

    explicit LookupTable(const CKKSCryptoContext& context, const CKKSPublicKey& publicKey)
        : CKKSComponent(context, publicKey)
    { }

    LookupTable(const LookupTable&) = default;
    LookupTable(LookupTable&&) = default;
    LookupTable& operator=(const LookupTable&) = default;
    LookupTable& operator=(LookupTable&&) = default;

private:
    CKKSCiphertext eval(CKKSCryptoContext cc, CKKSPublicKey publicKey, const CKKSArguments& args) override
    {
        auto arrayC = args.ciphertexts().at(0);
        auto indexC = args.ciphertexts().at(1);

        auto tempIndexCtx = cc->EvalRotate(indexC, -2047);

        int rotateIndex = 1;
        for (int i = 0; i < 11; i++)
        {
            cc->EvalAddInPlace(tempIndexCtx, cc->EvalRotate(tempIndexCtx, rotateIndex));
            rotateIndex <<= 1;
        }

        auto eqCtx = evalEqual(cc, publicKey, std::move(tempIndexCtx));
        auto tempResultCtx = cc->EvalMult(std::move(eqCtx), arrayC);

        rotateIndex = 1;
        for (int i = 0; i < 11; i++)
        {
            cc->EvalAddInPlace(tempResultCtx, cc->EvalRotate(tempResultCtx, rotateIndex));
            rotateIndex <<= 1;
        }

        return tempResultCtx;
    }

    CKKSCiphertext evalEqual(CKKSCryptoContext cc, CKKSPublicKey publicKey, CKKSCiphertext&& base) const
    {
        auto result = cc->EvalSquare(cc->EvalAdd(std::move(base), seqPtxt(cc)));

        for (int i = 0; i < 2; i++)
        {
            cc->EvalSquareInPlace(result);
        }

        std::vector<double> ones(2048, 1.0);
        auto onesPtxt = cc->MakeCKKSPackedPlaintext(ones);

        return cc->EvalSub(onesPtxt, std::move(result));
    }

    lbcrypto::Plaintext seqPtxt(CKKSCryptoContext cc) const
    {
        std::vector<double> result;
        result.reserve(2048);
        for (int64_t i = 0; i < 2048; ++i)
        {
            result.push_back(static_cast<double>(-i));
        }
        return cc->MakeCKKSPackedPlaintext(result);
    }
};

} // algo
} // polycircuit