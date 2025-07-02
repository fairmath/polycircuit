#pragma once

#include "polycircuit/IComponent.hpp"

#include "openfhe/pke/cryptocontext.h"

namespace polycircuit
{

template <typename ElementType>
class ParityChallenge final : public IComponent
{
public:
    explicit ParityChallenge(lbcrypto::CryptoContext<ElementType>&& cc,
                             lbcrypto::Ciphertext<ElementType>&& inputC)
        : m_cc(std::move(cc))
        , m_inputC(std::move(inputC))
    { }
    explicit ParityChallenge(const lbcrypto::CryptoContext<ElementType>& cc,
                             const lbcrypto::Ciphertext<ElementType>& inputC)
        : m_cc(cc)
        , m_inputC(inputC)
    { }
    ParityChallenge(const ParityChallenge&) = default;
    ParityChallenge(ParityChallenge&&) = default;
    ParityChallenge& operator=(const ParityChallenge&) = default;
    ParityChallenge& operator=(ParityChallenge&&) = default;

    Ciphertext evaluate() override
    {
        // m_cc->GetScheme()->LevelReduceInternalInPlace(m_inputC, 11);
        auto T1 = m_cc->EvalSub(m_inputC, 127);
        T1 = m_cc->EvalMult(T1, 1.0 / 64);

        auto T2 = m_cc->EvalSquare(T1);
        T2 = m_cc->EvalSub(T2, 2);

        auto T4 = m_cc->EvalSquare(T2);
        T4 = m_cc->EvalSub(T4, 2);

        auto T6 = m_cc->EvalMultNoRelin(T2, T4);
        T6 = m_cc->EvalSub(T6, T2);

        auto T8 = m_cc->EvalMultNoRelin(T4, T4);
        T8 = m_cc->EvalSub(T8, 2);

        m_cc->GetScheme()->LevelReduceInternalInPlace(T2, 2);
        T2 = m_cc->EvalMult(T2, 1.920896517349148 / 2);
        m_cc->GetScheme()->LevelReduceInternalInPlace(T4, 1);
        T4 = m_cc->EvalMult(T4, -0.5991976983070463 / 2);
        T6 = m_cc->EvalMult(T6, 0.05755941763614668 / 2);
        T8 = m_cc->EvalMult(T8, -0.0027561836569760636 / 2);

        lbcrypto::Ciphertext<ElementType> outputC = m_cc->EvalAdd(T2, T4);
        outputC = m_cc->EvalAdd(outputC, T6);
        outputC = m_cc->EvalAdd(outputC, T8);
        outputC = m_cc->EvalAdd(outputC, 0.6019539733666786);
        m_cc->RelinearizeInPlace(outputC);

        double c = 0.9892280131939755;
        for (int i = 0; i < 6; i++)
        {
            c = c * c;
            outputC = m_cc->EvalSquare(outputC);
            outputC = m_cc->EvalSub(outputC, c + c);
        }
        c = c * c;
        outputC = m_cc->EvalMultNoRelin(outputC, outputC);
        outputC = m_cc->EvalAdd(outputC, 0.5 - c - c);
        return Ciphertext(std::move(outputC));
    }

private:
    lbcrypto::CryptoContext<ElementType> m_cc;
    lbcrypto::Ciphertext<ElementType> m_inputC;
};

} // polycircuit

