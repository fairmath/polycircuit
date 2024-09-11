#pragma once

#include "polycircuit/IComponent.hpp"

#include "openfhe/pke/cryptocontext.h"

#include <list>

namespace polycircuit
{

template <typename ElementType>
class CIFAR10ImageClassification final : public IComponent
{
public:
    explicit CIFAR10ImageClassification(lbcrypto::CryptoContext<ElementType>&& cc,
                                        lbcrypto::Ciphertext<ElementType>&& inputC)
        : m_cc(std::move(cc))
        , m_inputC(std::move(inputC))
    { }
    explicit CIFAR10ImageClassification(const lbcrypto::CryptoContext<ElementType>& cc,
                                        const lbcrypto::Ciphertext<ElementType>& inputC)
        : m_cc(cc)
        , m_inputC(inputC)
    { }
    CIFAR10ImageClassification(const CIFAR10ImageClassification&) = default;
    CIFAR10ImageClassification(CIFAR10ImageClassification&&) = default;
    CIFAR10ImageClassification& operator=(const CIFAR10ImageClassification&) = default;
    CIFAR10ImageClassification& operator=(CIFAR10ImageClassification&&) = default;

    Ciphertext evaluate() override
    {
        std::list<std::vector<double>> C =
        {
            #include "CIFAR10ImageClassification_weights.txt"
        };

        std::vector<lbcrypto::Ciphertext<ElementType>> T(9);
        std::vector<double> V;
        V.reserve(4096);

        V.insert(V.cend(), C.front().cbegin(), C.front().cend());
        C.pop_front();

        auto input_vec = m_inputC;
        input_vec = m_cc->EvalAdd(input_vec, m_cc->MakeCKKSPackedPlaintext(V));
        V.clear();

        V.insert(V.cend(), C.front().cbegin(), C.front().cend());
        C.pop_front();
        input_vec = m_cc->EvalMult(input_vec, m_cc->MakeCKKSPackedPlaintext(V));
        V.clear();

        V.insert(V.cend(), C.front().cbegin(), C.front().cend());
        C.pop_front();

        T[1] = input_vec->Clone();
        T[2] = m_cc->EvalMult(T[1], T[1]);
        T[2] = m_cc->EvalAdd(T[2], T[2]);
        T[2] = m_cc->EvalSub(T[2], 1);
        T[3] = m_cc->EvalMult(T[2], T[1]);
        T[3] = m_cc->EvalAdd(T[3], T[3]);
        T[3] = m_cc->EvalSub(T[3], T[1]);
        T[4] = m_cc->EvalMult(T[2], T[2]);
        T[4] = m_cc->EvalAdd(T[4], T[4]);
        T[4] = m_cc->EvalSub(T[4], 1);
        T[5] = m_cc->EvalMult(T[4], T[1]);
        T[5] = m_cc->EvalAdd(T[5], T[5]);
        T[5] = m_cc->EvalSub(T[5], T[3]);
        T[6] = m_cc->EvalMult(T[4], T[2]);
        T[6] = m_cc->EvalAdd(T[6], T[6]);
        T[6] = m_cc->EvalSub(T[6], T[2]);
        T[7] = m_cc->EvalMult(T[4], T[3]);
        T[7] = m_cc->EvalAdd(T[7], T[7]);
        T[7] = m_cc->EvalSub(T[7], T[1]);
        T[8] = m_cc->EvalMult(T[4], T[4]);
        T[8] = m_cc->EvalAdd(T[8], T[8]);
        T[8] = m_cc->EvalSub(T[8], 1);

        lbcrypto::Ciphertext<ElementType> tmp = m_cc->EvalMult(T[1], m_cc->MakeCKKSPackedPlaintext(V));
        V.clear();

        V.insert(V.cend(), C.front().cbegin(), C.front().cend());
        C.pop_front();
        lbcrypto::Ciphertext<ElementType> enc_out = m_cc->EvalAdd(tmp, m_cc->MakeCKKSPackedPlaintext(V));
        V.clear();

        V.insert(V.cend(), C.front().cbegin(), C.front().cend());
        C.pop_front();
        tmp = m_cc->EvalMult(T[1], m_cc->MakeCKKSPackedPlaintext(V));
        V.clear();

        V.insert(V.cend(), C.front().cbegin(), C.front().cend());
        C.pop_front();
        tmp = m_cc->EvalRotate(tmp, -1);
        enc_out = m_cc->EvalAdd(tmp, enc_out);
        tmp = m_cc->EvalMult(T[1], m_cc->MakeCKKSPackedPlaintext(V));
        V.clear();

        V.insert(V.cend(), C.front().cbegin(), C.front().cend());
        C.pop_front();
        tmp = m_cc->EvalRotate(tmp, -2);
        enc_out = m_cc->EvalAdd(tmp, enc_out);
        tmp = m_cc->EvalMult(T[1], m_cc->MakeCKKSPackedPlaintext(V));
        V.clear();

        V.insert(V.cend(), C.front().cbegin(), C.front().cend());
        C.pop_front();
        tmp = m_cc->EvalRotate(tmp, -3);
        enc_out = m_cc->EvalAdd(tmp, enc_out);
        tmp = m_cc->EvalMult(T[2], m_cc->MakeCKKSPackedPlaintext(V));
        V.clear();

        V.insert(V.cend(), C.front().cbegin(), C.front().cend());
        C.pop_front();
        enc_out = m_cc->EvalAdd(tmp, enc_out);
        tmp = m_cc->EvalMult(T[2], m_cc->MakeCKKSPackedPlaintext(V));
        V.clear();

        V.insert(V.cend(), C.front().cbegin(), C.front().cend());
        C.pop_front();
        tmp = m_cc->EvalRotate(tmp, -1);
        enc_out = m_cc->EvalAdd(tmp, enc_out);
        tmp = m_cc->EvalMult(T[2], m_cc->MakeCKKSPackedPlaintext(V));
        V.clear();

        V.insert(V.cend(), C.front().cbegin(), C.front().cend());
        C.pop_front();
        tmp = m_cc->EvalRotate(tmp, -2);
        enc_out = m_cc->EvalAdd(tmp, enc_out);
        tmp = m_cc->EvalMult(T[2], m_cc->MakeCKKSPackedPlaintext(V));
        V.clear();

        V.insert(V.cend(), C.front().cbegin(), C.front().cend());
        C.pop_front();
        tmp = m_cc->EvalRotate(tmp, -3);
        enc_out = m_cc->EvalAdd(tmp, enc_out);
        tmp = m_cc->EvalMult(T[3], m_cc->MakeCKKSPackedPlaintext(V));
        V.clear();

        V.insert(V.cend(), C.front().cbegin(), C.front().cend());
        C.pop_front();
        enc_out = m_cc->EvalAdd(tmp, enc_out);
        tmp = m_cc->EvalMult(T[3], m_cc->MakeCKKSPackedPlaintext(V));
        V.clear();

        V.insert(V.cend(), C.front().cbegin(), C.front().cend());
        C.pop_front();
        tmp = m_cc->EvalRotate(tmp, -1);
        enc_out = m_cc->EvalAdd(tmp, enc_out);
        tmp = m_cc->EvalMult(T[3], m_cc->MakeCKKSPackedPlaintext(V));
        V.clear();

        V.insert(V.cend(), C.front().cbegin(), C.front().cend());
        C.pop_front();
        tmp = m_cc->EvalRotate(tmp, -2);
        enc_out = m_cc->EvalAdd(tmp, enc_out);
        tmp = m_cc->EvalMult(T[3], m_cc->MakeCKKSPackedPlaintext(V));
        V.clear();

        V.insert(V.cend(), C.front().cbegin(), C.front().cend());
        C.pop_front();
        tmp = m_cc->EvalRotate(tmp, -3);
        enc_out = m_cc->EvalAdd(tmp, enc_out);
        tmp = m_cc->EvalMult(T[4], m_cc->MakeCKKSPackedPlaintext(V));
        V.clear();

        V.insert(V.cend(), C.front().cbegin(), C.front().cend());
        C.pop_front();
        enc_out = m_cc->EvalAdd(tmp, enc_out);
        tmp = m_cc->EvalMult(T[4], m_cc->MakeCKKSPackedPlaintext(V));
        V.clear();

        V.insert(V.cend(), C.front().cbegin(), C.front().cend());
        C.pop_front();
        tmp = m_cc->EvalRotate(tmp, -1);
        enc_out = m_cc->EvalAdd(tmp, enc_out);
        tmp = m_cc->EvalMult(T[4], m_cc->MakeCKKSPackedPlaintext(V));
        V.clear();

        V.insert(V.cend(), C.front().cbegin(), C.front().cend());
        C.pop_front();
        tmp = m_cc->EvalRotate(tmp, -2);
        enc_out = m_cc->EvalAdd(tmp, enc_out);
        tmp = m_cc->EvalMult(T[4], m_cc->MakeCKKSPackedPlaintext(V));
        V.clear();

        V.insert(V.cend(), C.front().cbegin(), C.front().cend());
        C.pop_front();
        tmp = m_cc->EvalRotate(tmp, -3);
        enc_out = m_cc->EvalAdd(tmp, enc_out);
        tmp = m_cc->EvalMult(T[5], m_cc->MakeCKKSPackedPlaintext(V));
        V.clear();

        V.insert(V.cend(), C.front().cbegin(), C.front().cend());
        C.pop_front();
        enc_out = m_cc->EvalAdd(tmp, enc_out);
        tmp = m_cc->EvalMult(T[5], m_cc->MakeCKKSPackedPlaintext(V));
        V.clear();

        V.insert(V.cend(), C.front().cbegin(), C.front().cend());
        C.pop_front();
        tmp = m_cc->EvalRotate(tmp, -1);
        enc_out = m_cc->EvalAdd(tmp, enc_out);
        tmp = m_cc->EvalMult(T[5], m_cc->MakeCKKSPackedPlaintext(V));
        V.clear();

        V.insert(V.cend(), C.front().cbegin(), C.front().cend());
        C.pop_front();
        tmp = m_cc->EvalRotate(tmp, -2);
        enc_out = m_cc->EvalAdd(tmp, enc_out);
        tmp = m_cc->EvalMult(T[5], m_cc->MakeCKKSPackedPlaintext(V));
        V.clear();

        V.insert(V.cend(), C.front().cbegin(), C.front().cend());
        C.pop_front();
        tmp = m_cc->EvalRotate(tmp, -3);
        enc_out = m_cc->EvalAdd(tmp, enc_out);
        tmp = m_cc->EvalMult(T[6], m_cc->MakeCKKSPackedPlaintext(V));
        V.clear();

        V.insert(V.cend(), C.front().cbegin(), C.front().cend());
        C.pop_front();
        enc_out = m_cc->EvalAdd(tmp, enc_out);
        tmp = m_cc->EvalMult(T[6], m_cc->MakeCKKSPackedPlaintext(V));
        V.clear();

        V.insert(V.cend(), C.front().cbegin(), C.front().cend());
        C.pop_front();
        tmp = m_cc->EvalRotate(tmp, -1);
        enc_out = m_cc->EvalAdd(tmp, enc_out);
        tmp = m_cc->EvalMult(T[6], m_cc->MakeCKKSPackedPlaintext(V));
        V.clear();

        V.insert(V.cend(), C.front().cbegin(), C.front().cend());
        C.pop_front();
        tmp = m_cc->EvalRotate(tmp, -2);
        enc_out = m_cc->EvalAdd(tmp, enc_out);
        tmp = m_cc->EvalMult(T[6], m_cc->MakeCKKSPackedPlaintext(V));
        V.clear();

        V.insert(V.cend(), C.front().cbegin(), C.front().cend());
        C.pop_front();
        tmp = m_cc->EvalRotate(tmp, -3);
        enc_out = m_cc->EvalAdd(tmp, enc_out);
        tmp = m_cc->EvalMult(T[7], m_cc->MakeCKKSPackedPlaintext(V));
        V.clear();

        V.insert(V.cend(), C.front().cbegin(), C.front().cend());
        C.pop_front();
        enc_out = m_cc->EvalAdd(tmp, enc_out);
        tmp = m_cc->EvalMult(T[7], m_cc->MakeCKKSPackedPlaintext(V));
        V.clear();

        V.insert(V.cend(), C.front().cbegin(), C.front().cend());
        C.pop_front();
        tmp = m_cc->EvalRotate(tmp, -1);
        enc_out = m_cc->EvalAdd(tmp, enc_out);
        tmp = m_cc->EvalMult(T[7], m_cc->MakeCKKSPackedPlaintext(V));
        V.clear();

        V.insert(V.cend(), C.front().cbegin(), C.front().cend());
        C.pop_front();
        tmp = m_cc->EvalRotate(tmp, -2);
        enc_out = m_cc->EvalAdd(tmp, enc_out);
        tmp = m_cc->EvalMult(T[7], m_cc->MakeCKKSPackedPlaintext(V));
        V.clear();

        V.insert(V.cend(), C.front().cbegin(), C.front().cend());
        C.pop_front();
        tmp = m_cc->EvalRotate(tmp, -3);
        enc_out = m_cc->EvalAdd(tmp, enc_out);
        tmp = m_cc->EvalMult(T[8], m_cc->MakeCKKSPackedPlaintext(V));
        V.clear();

        V.insert(V.cend(), C.front().cbegin(), C.front().cend());
        C.pop_front();
        enc_out = m_cc->EvalAdd(tmp, enc_out);
        tmp = m_cc->EvalMult(T[8], m_cc->MakeCKKSPackedPlaintext(V));
        V.clear();

        V.insert(V.cend(), C.front().cbegin(), C.front().cend());
        C.pop_front();
        tmp = m_cc->EvalRotate(tmp, -1);
        enc_out = m_cc->EvalAdd(tmp, enc_out);
        tmp = m_cc->EvalMult(T[8], m_cc->MakeCKKSPackedPlaintext(V));
        V.clear();

        V.insert(V.cend(), C.front().cbegin(), C.front().cend());

        tmp = m_cc->EvalRotate(tmp, -2);
        enc_out = m_cc->EvalAdd(tmp, enc_out);
        tmp = m_cc->EvalMult(T[8], m_cc->MakeCKKSPackedPlaintext(V));
        tmp = m_cc->EvalRotate(tmp, -3);
        enc_out = m_cc->EvalAdd(tmp, enc_out);
        tmp = m_cc->EvalRotate(enc_out, 1600);
        enc_out = m_cc->EvalAdd(enc_out, tmp);
        tmp = m_cc->EvalRotate(enc_out, 800);
        enc_out = m_cc->EvalAdd(enc_out, tmp);
        tmp = m_cc->EvalRotate(enc_out, 400);
        enc_out = m_cc->EvalAdd(enc_out, tmp);
        tmp = m_cc->EvalRotate(enc_out, 200);
        enc_out = m_cc->EvalAdd(enc_out, tmp);
        tmp = m_cc->EvalRotate(enc_out, 100);
        enc_out = m_cc->EvalAdd(enc_out, tmp);
        tmp = m_cc->EvalRotate(enc_out, 50);
        enc_out = m_cc->EvalAdd(enc_out, tmp);
        lbcrypto::Ciphertext<ElementType> enc_out1 = m_cc->EvalRotate(enc_out, 40);
        tmp = m_cc->EvalRotate(enc_out, 20);
        enc_out = m_cc->EvalAdd(enc_out, tmp);
        tmp = m_cc->EvalRotate(enc_out, 10);
        enc_out = m_cc->EvalAdd(enc_out, tmp);
        enc_out = m_cc->EvalAdd(enc_out, enc_out1);

        return Ciphertext(std::move(enc_out));
    }

private:
    lbcrypto::CryptoContext<ElementType> m_cc;
    lbcrypto::Ciphertext<ElementType> m_inputC;
};

} // polycircuit

