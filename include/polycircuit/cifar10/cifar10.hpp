#pragma once

#include "polycircuit/icomponent.hpp"

#include "openfhe/pke/cryptocontext.h"

namespace polycircuit {
namespace ckks {
template <typename ElementType>
class Cifar10 final : public IComponent {
public:
    explicit Cifar10(lbcrypto::CryptoContext<ElementType>&& cc, lbcrypto::Ciphertext<ElementType>&& inputC)
        : m_cc(std::move(cc)), m_inputC(std::move(inputC)) {}
    explicit Cifar10(const lbcrypto::CryptoContext<ElementType>& cc, const lbcrypto::Ciphertext<ElementType>& inputC)
        : m_cc(cc), m_inputC(inputC) {}
    Cifar10(const Cifar10&)            = default;
    Cifar10(Cifar10&&)                 = default;
    Cifar10& operator=(const Cifar10&) = default;
    Cifar10& operator=(Cifar10&&)      = default;

    Ciphertext evaluate() override {
        const std::vector<std::vector<double>> weightsMatrix{
#include "weights.txt"
        };

        m_inputC = m_cc->EvalAdd(m_inputC, m_cc->MakeCKKSPackedPlaintext(weightsMatrix.at(0)));
        m_inputC = m_cc->EvalMult(m_inputC, m_cc->MakeCKKSPackedPlaintext(weightsMatrix.at(1)));

        lbcrypto::Ciphertext<ElementType> tmp =
            m_cc->EvalMult(m_inputC, m_cc->MakeCKKSPackedPlaintext(weightsMatrix.at(2)));
        lbcrypto::Ciphertext<ElementType> enc_out =
            m_cc->EvalAdd(tmp, m_cc->MakeCKKSPackedPlaintext(weightsMatrix.at(3)));
        tmp     = m_cc->EvalMult(m_inputC, m_cc->MakeCKKSPackedPlaintext(weightsMatrix.at(4)));
        tmp     = m_cc->EvalRotate(tmp, -1);
        enc_out = m_cc->EvalAdd(tmp, enc_out);
        tmp     = m_cc->EvalMult(m_inputC, m_cc->MakeCKKSPackedPlaintext(weightsMatrix.at(5)));
        tmp     = m_cc->EvalRotate(tmp, -2);
        enc_out = m_cc->EvalAdd(tmp, enc_out);
        tmp     = m_cc->EvalMult(m_inputC, m_cc->MakeCKKSPackedPlaintext(weightsMatrix.at(6)));
        tmp     = m_cc->EvalRotate(tmp, -3);
        enc_out = m_cc->EvalAdd(tmp, enc_out);

        lbcrypto::Ciphertext<ElementType> t0 = m_cc->EvalMult(m_inputC, m_inputC);
        t0                                   = m_cc->EvalAdd(t0, t0);
        t0                                   = m_cc->EvalSub(t0, 1);
        tmp                                  = m_cc->EvalMult(t0, m_cc->MakeCKKSPackedPlaintext(weightsMatrix.at(7)));
        enc_out                              = m_cc->EvalAdd(tmp, enc_out);
        tmp                                  = m_cc->EvalMult(t0, m_cc->MakeCKKSPackedPlaintext(weightsMatrix.at(8)));
        tmp                                  = m_cc->EvalRotate(tmp, -1);
        enc_out                              = m_cc->EvalAdd(tmp, enc_out);
        tmp                                  = m_cc->EvalMult(t0, m_cc->MakeCKKSPackedPlaintext(weightsMatrix.at(9)));
        tmp                                  = m_cc->EvalRotate(tmp, -2);
        enc_out                              = m_cc->EvalAdd(tmp, enc_out);
        tmp                                  = m_cc->EvalMult(t0, m_cc->MakeCKKSPackedPlaintext(weightsMatrix.at(10)));
        tmp                                  = m_cc->EvalRotate(tmp, -3);
        enc_out                              = m_cc->EvalAdd(tmp, enc_out);

        lbcrypto::Ciphertext<ElementType> t1 = m_cc->EvalMult(t0, m_inputC);
        t1                                   = m_cc->EvalAdd(t1, t1);
        t1                                   = m_cc->EvalSub(t1, m_inputC);
        tmp                                  = m_cc->EvalMult(t1, m_cc->MakeCKKSPackedPlaintext(weightsMatrix.at(11)));
        enc_out                              = m_cc->EvalAdd(tmp, enc_out);
        tmp                                  = m_cc->EvalMult(t1, m_cc->MakeCKKSPackedPlaintext(weightsMatrix.at(12)));
        tmp                                  = m_cc->EvalRotate(tmp, -1);
        enc_out                              = m_cc->EvalAdd(tmp, enc_out);
        tmp                                  = m_cc->EvalMult(t1, m_cc->MakeCKKSPackedPlaintext(weightsMatrix.at(13)));
        tmp                                  = m_cc->EvalRotate(tmp, -2);
        enc_out                              = m_cc->EvalAdd(tmp, enc_out);
        tmp                                  = m_cc->EvalMult(t1, m_cc->MakeCKKSPackedPlaintext(weightsMatrix.at(14)));
        tmp                                  = m_cc->EvalRotate(tmp, -3);
        enc_out                              = m_cc->EvalAdd(tmp, enc_out);

        lbcrypto::Ciphertext<ElementType> t2 = m_cc->EvalMult(t0, t0);
        t2                                   = m_cc->EvalAdd(t2, t2);
        t2                                   = m_cc->EvalSub(t2, 1);
        tmp                                  = m_cc->EvalMult(t2, m_cc->MakeCKKSPackedPlaintext(weightsMatrix.at(15)));
        enc_out                              = m_cc->EvalAdd(tmp, enc_out);
        tmp                                  = m_cc->EvalMult(t2, m_cc->MakeCKKSPackedPlaintext(weightsMatrix.at(16)));
        tmp                                  = m_cc->EvalRotate(tmp, -1);
        enc_out                              = m_cc->EvalAdd(tmp, enc_out);
        tmp                                  = m_cc->EvalMult(t2, m_cc->MakeCKKSPackedPlaintext(weightsMatrix.at(17)));
        tmp                                  = m_cc->EvalRotate(tmp, -2);
        enc_out                              = m_cc->EvalAdd(tmp, enc_out);
        tmp                                  = m_cc->EvalMult(t2, m_cc->MakeCKKSPackedPlaintext(weightsMatrix.at(18)));
        tmp                                  = m_cc->EvalRotate(tmp, -3);
        enc_out                              = m_cc->EvalAdd(tmp, enc_out);

        lbcrypto::Ciphertext<ElementType> t3 = m_cc->EvalMult(t2, m_inputC);
        t3                                   = m_cc->EvalAdd(t3, t3);
        t3                                   = m_cc->EvalSub(t3, t1);
        tmp                                  = m_cc->EvalMult(t3, m_cc->MakeCKKSPackedPlaintext(weightsMatrix.at(19)));
        enc_out                              = m_cc->EvalAdd(tmp, enc_out);
        tmp                                  = m_cc->EvalMult(t3, m_cc->MakeCKKSPackedPlaintext(weightsMatrix.at(20)));
        tmp                                  = m_cc->EvalRotate(tmp, -1);
        enc_out                              = m_cc->EvalAdd(tmp, enc_out);
        tmp                                  = m_cc->EvalMult(t3, m_cc->MakeCKKSPackedPlaintext(weightsMatrix.at(21)));
        tmp                                  = m_cc->EvalRotate(tmp, -2);
        enc_out                              = m_cc->EvalAdd(tmp, enc_out);
        tmp                                  = m_cc->EvalMult(t3, m_cc->MakeCKKSPackedPlaintext(weightsMatrix.at(22)));
        tmp                                  = m_cc->EvalRotate(tmp, -3);
        enc_out                              = m_cc->EvalAdd(tmp, enc_out);

        lbcrypto::Ciphertext<ElementType> t4 = m_cc->EvalMult(t2, t0);
        t4                                   = m_cc->EvalAdd(t4, t4);
        t4                                   = m_cc->EvalSub(t4, t0);
        tmp                                  = m_cc->EvalMult(t4, m_cc->MakeCKKSPackedPlaintext(weightsMatrix.at(23)));
        enc_out                              = m_cc->EvalAdd(tmp, enc_out);
        tmp                                  = m_cc->EvalMult(t4, m_cc->MakeCKKSPackedPlaintext(weightsMatrix.at(24)));
        tmp                                  = m_cc->EvalRotate(tmp, -1);
        enc_out                              = m_cc->EvalAdd(tmp, enc_out);
        tmp                                  = m_cc->EvalMult(t4, m_cc->MakeCKKSPackedPlaintext(weightsMatrix.at(25)));
        tmp                                  = m_cc->EvalRotate(tmp, -2);
        enc_out                              = m_cc->EvalAdd(tmp, enc_out);
        tmp                                  = m_cc->EvalMult(t4, m_cc->MakeCKKSPackedPlaintext(weightsMatrix.at(26)));
        tmp                                  = m_cc->EvalRotate(tmp, -3);
        enc_out                              = m_cc->EvalAdd(tmp, enc_out);

        t0      = m_cc->EvalMult(t2, t1);
        t0      = m_cc->EvalAdd(t0, t0);
        t0      = m_cc->EvalSub(t0, m_inputC);
        tmp     = m_cc->EvalMult(t0, m_cc->MakeCKKSPackedPlaintext(weightsMatrix.at(27)));
        enc_out = m_cc->EvalAdd(tmp, enc_out);
        tmp     = m_cc->EvalMult(t0, m_cc->MakeCKKSPackedPlaintext(weightsMatrix.at(28)));
        tmp     = m_cc->EvalRotate(tmp, -1);
        enc_out = m_cc->EvalAdd(tmp, enc_out);
        tmp     = m_cc->EvalMult(t0, m_cc->MakeCKKSPackedPlaintext(weightsMatrix.at(29)));
        tmp     = m_cc->EvalRotate(tmp, -2);
        enc_out = m_cc->EvalAdd(tmp, enc_out);
        tmp     = m_cc->EvalMult(t0, m_cc->MakeCKKSPackedPlaintext(weightsMatrix.at(30)));
        tmp     = m_cc->EvalRotate(tmp, -3);
        enc_out = m_cc->EvalAdd(tmp, enc_out);

        t1      = m_cc->EvalMult(t2, t2);
        t1      = m_cc->EvalAdd(t1, t1);
        t1      = m_cc->EvalSub(t1, 1);
        tmp     = m_cc->EvalMult(t1, m_cc->MakeCKKSPackedPlaintext(weightsMatrix.at(31)));
        enc_out = m_cc->EvalAdd(tmp, enc_out);
        tmp     = m_cc->EvalMult(t1, m_cc->MakeCKKSPackedPlaintext(weightsMatrix.at(32)));
        tmp     = m_cc->EvalRotate(tmp, -1);
        enc_out = m_cc->EvalAdd(tmp, enc_out);
        tmp     = m_cc->EvalMult(t1, m_cc->MakeCKKSPackedPlaintext(weightsMatrix.at(33)));
        tmp     = m_cc->EvalRotate(tmp, -2);
        enc_out = m_cc->EvalAdd(tmp, enc_out);
        tmp     = m_cc->EvalMult(t1, m_cc->MakeCKKSPackedPlaintext(weightsMatrix.at(34)));
        tmp     = m_cc->EvalRotate(tmp, -3);
        enc_out = m_cc->EvalAdd(tmp, enc_out);
        tmp     = m_cc->EvalRotate(enc_out, 1600);
        enc_out = m_cc->EvalAdd(enc_out, tmp);
        tmp     = m_cc->EvalRotate(enc_out, 800);
        enc_out = m_cc->EvalAdd(enc_out, tmp);
        tmp     = m_cc->EvalRotate(enc_out, 400);
        enc_out = m_cc->EvalAdd(enc_out, tmp);
        tmp     = m_cc->EvalRotate(enc_out, 200);
        enc_out = m_cc->EvalAdd(enc_out, tmp);
        tmp     = m_cc->EvalRotate(enc_out, 100);
        enc_out = m_cc->EvalAdd(enc_out, tmp);
        tmp     = m_cc->EvalRotate(enc_out, 50);
        enc_out = m_cc->EvalAdd(enc_out, tmp);
        lbcrypto::Ciphertext<ElementType> enc_out1 = m_cc->EvalRotate(enc_out, 40);
        tmp                                        = m_cc->EvalRotate(enc_out, 20);
        enc_out                                    = m_cc->EvalAdd(enc_out, tmp);
        tmp                                        = m_cc->EvalRotate(enc_out, 10);
        enc_out                                    = m_cc->EvalAdd(enc_out, tmp);
        enc_out                                    = m_cc->EvalAdd(enc_out, enc_out1);

        return Ciphertext(std::move(enc_out));
    }

private:
    lbcrypto::CryptoContext<ElementType> m_cc;
    lbcrypto::Ciphertext<ElementType> m_inputC;
};
}  // namespace ckks
}  // namespace polycircuit
