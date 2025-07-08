#pragma once

#include "polycircuit/context.hpp"
#include "polycircuit/IComponent.hpp"

#include "openfhe/pke/cryptocontext.h"
#include "openfhe/pke/scheme/ckksrns/gen-cryptocontext-ckksrns-params.h"

namespace polycircuit
{

namespace func {

CKKSParams parityDefaultParams() 
{
    lbcrypto::CCParams<lbcrypto::CryptoContextCKKSRNS> parameters;

    parameters.SetMultiplicativeDepth(12);
    parameters.SetScalingModSize(46);
    parameters.SetFirstModSize(55);
    parameters.SetScalingTechnique(lbcrypto::ScalingTechnique::FIXEDAUTO);
    parameters.SetBatchSize(65536);
    parameters.SetRingDim(131072);

    return {parameters, {}};
} 

class Parity final : public CKKSComponent
{
public:
    explicit Parity(CKKSCryptoContext&& context, CKKSPublicKey&& publicKey)
        : CKKSComponent(std::move(context), std::move(publicKey))
    { }

    explicit Parity(const CKKSCryptoContext& context, const CKKSPublicKey& publicKey)
        : CKKSComponent(context, publicKey)
    { }

    Parity(const Parity&) = default;
    Parity(Parity&&) = default;
    Parity& operator=(const Parity&) = default;
    Parity& operator=(Parity&&) = default;

private:
    CKKSCiphertext eval(CKKSCryptoContext cc, CKKSPublicKey publicKey, const CKKSArguments& args) override
    {
        auto inputC = args.ciphertexts().at(0);

        // cc->GetScheme()->LevelReduceInternalInPlace(m_inputC, 11);
        auto T1 = cc->EvalSub(inputC, 127);
        T1 = cc->EvalMult(T1, 1.0 / 64);

        auto T2 = cc->EvalSquare(T1);
        T2 = cc->EvalSub(T2, 2);

        auto T4 = cc->EvalSquare(T2);
        T4 = cc->EvalSub(T4, 2);

        auto T6 = cc->EvalMultNoRelin(T2, T4);
        T6 = cc->EvalSub(T6, T2);

        auto T8 = cc->EvalMultNoRelin(T4, T4);
        T8 = cc->EvalSub(T8, 2);

        cc->GetScheme()->LevelReduceInternalInPlace(T2, 2);
        T2 = cc->EvalMult(T2, 1.920896517349148 / 2);
        cc->GetScheme()->LevelReduceInternalInPlace(T4, 1);
        T4 = cc->EvalMult(T4, -0.5991976983070463 / 2);
        T6 = cc->EvalMult(T6, 0.05755941763614668 / 2);
        T8 = cc->EvalMult(T8, -0.0027561836569760636 / 2);

        auto outputC = cc->EvalAdd(T2, T4);
        outputC = cc->EvalAdd(outputC, T6);
        outputC = cc->EvalAdd(outputC, T8);
        outputC = cc->EvalAdd(outputC, 0.6019539733666786);
        cc->RelinearizeInPlace(outputC);

        double c = 0.9892280131939755;
        for (int i = 0; i < 6; i++)
        {
            c = c * c;
            outputC = cc->EvalSquare(outputC);
            outputC = cc->EvalSub(outputC, c + c);
        }
        c = c * c;
        outputC = cc->EvalMultNoRelin(outputC, outputC);
        outputC = cc->EvalAdd(outputC, 0.5 - c - c);

        return outputC;
    }
};

} // func
} // polycircuit
