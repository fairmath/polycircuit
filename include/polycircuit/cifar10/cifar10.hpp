#pragma once

#include "polycircuit/ckks_component.h"

#include "openfhe/pke/cryptocontext.h"

namespace polycircuit {
    namespace ckks {

        class Cifar10 final : public CKKSComponent {
        public:
            explicit Cifar10() {
                init();
            };

            explicit Cifar10(const lbcrypto::CryptoContext<lbcrypto::DCRTPoly> &cc,
                             const lbcrypto::Ciphertext<lbcrypto::DCRTPoly> &inputC) {
                init();
                SetCryptoContext(cc);
                AddCipher(inputC);
            }

            void init() {
                SetMultDepth(5);
                SetScaleModSize(25);
                SetFirstModSize(30);
                //cifar10Evaluator.SetScalingTechnique(lbcrypto::ScalingTechnique::FLEXIBLEAUTO);
                SetBatchSize(4096);
                SetRingDimension(8192);
                SetRotateKeyIndexes({-3, -2, -1, 10, 20, 40, 50, 100, 200, 400, 800, 1600});
            }

            Cifar10(const Cifar10 &) = default;

            Cifar10(Cifar10 &&) = default;

            Ciphertext evaluate() override {
                const std::vector<std::vector<double>> weightsMatrix{
#include "weights.txt"
                };
                auto m_inputC = GetInputs()[0];
                auto cc = GetCryptoContext();
                m_inputC = cc->EvalAdd(m_inputC, cc->MakeCKKSPackedPlaintext(weightsMatrix.at(0)));
                m_inputC = cc->EvalMult(m_inputC, cc->MakeCKKSPackedPlaintext(weightsMatrix.at(1)));

                auto tmp = cc->EvalMult(m_inputC, cc->MakeCKKSPackedPlaintext(weightsMatrix.at(2)));
                auto enc_out = cc->EvalAdd(tmp, cc->MakeCKKSPackedPlaintext(weightsMatrix.at(3)));
                tmp = cc->EvalMult(m_inputC, cc->MakeCKKSPackedPlaintext(weightsMatrix.at(4)));
                tmp = cc->EvalRotate(tmp, -1);
                enc_out = cc->EvalAdd(tmp, enc_out);
                tmp = cc->EvalMult(m_inputC, cc->MakeCKKSPackedPlaintext(weightsMatrix.at(5)));
                tmp = cc->EvalRotate(tmp, -2);
                enc_out = cc->EvalAdd(tmp, enc_out);
                tmp = cc->EvalMult(m_inputC, cc->MakeCKKSPackedPlaintext(weightsMatrix.at(6)));
                tmp = cc->EvalRotate(tmp, -3);
                enc_out = cc->EvalAdd(tmp, enc_out);

                auto t0 = cc->EvalMult(m_inputC, m_inputC);
                t0 = cc->EvalAdd(t0, t0);
                t0 = cc->EvalSub(t0, 1);
                tmp = cc->EvalMult(t0, cc->MakeCKKSPackedPlaintext(weightsMatrix.at(7)));
                enc_out = cc->EvalAdd(tmp, enc_out);
                tmp = cc->EvalMult(t0, cc->MakeCKKSPackedPlaintext(weightsMatrix.at(8)));
                tmp = cc->EvalRotate(tmp, -1);
                enc_out = cc->EvalAdd(tmp, enc_out);
                tmp = cc->EvalMult(t0, cc->MakeCKKSPackedPlaintext(weightsMatrix.at(9)));
                tmp = cc->EvalRotate(tmp, -2);
                enc_out = cc->EvalAdd(tmp, enc_out);
                tmp = cc->EvalMult(t0, cc->MakeCKKSPackedPlaintext(weightsMatrix.at(10)));
                tmp = cc->EvalRotate(tmp, -3);
                enc_out = cc->EvalAdd(tmp, enc_out);

                auto t1 = cc->EvalMult(t0, m_inputC);
                t1 = cc->EvalAdd(t1, t1);
                t1 = cc->EvalSub(t1, m_inputC);
                tmp = cc->EvalMult(t1, cc->MakeCKKSPackedPlaintext(weightsMatrix.at(11)));
                enc_out = cc->EvalAdd(tmp, enc_out);
                tmp = cc->EvalMult(t1, cc->MakeCKKSPackedPlaintext(weightsMatrix.at(12)));
                tmp = cc->EvalRotate(tmp, -1);
                enc_out = cc->EvalAdd(tmp, enc_out);
                tmp = cc->EvalMult(t1, cc->MakeCKKSPackedPlaintext(weightsMatrix.at(13)));
                tmp = cc->EvalRotate(tmp, -2);
                enc_out = cc->EvalAdd(tmp, enc_out);
                tmp = cc->EvalMult(t1, cc->MakeCKKSPackedPlaintext(weightsMatrix.at(14)));
                tmp = cc->EvalRotate(tmp, -3);
                enc_out = cc->EvalAdd(tmp, enc_out);

                auto t2 = cc->EvalMult(t0, t0);
                t2 = cc->EvalAdd(t2, t2);
                t2 = cc->EvalSub(t2, 1);
                tmp = cc->EvalMult(t2, cc->MakeCKKSPackedPlaintext(weightsMatrix.at(15)));
                enc_out = cc->EvalAdd(tmp, enc_out);
                tmp = cc->EvalMult(t2, cc->MakeCKKSPackedPlaintext(weightsMatrix.at(16)));
                tmp = cc->EvalRotate(tmp, -1);
                enc_out = cc->EvalAdd(tmp, enc_out);
                tmp = cc->EvalMult(t2, cc->MakeCKKSPackedPlaintext(weightsMatrix.at(17)));
                tmp = cc->EvalRotate(tmp, -2);
                enc_out = cc->EvalAdd(tmp, enc_out);
                tmp = cc->EvalMult(t2, cc->MakeCKKSPackedPlaintext(weightsMatrix.at(18)));
                tmp = cc->EvalRotate(tmp, -3);
                enc_out = cc->EvalAdd(tmp, enc_out);

                auto t3 = cc->EvalMult(t2, m_inputC);
                t3 = cc->EvalAdd(t3, t3);
                t3 = cc->EvalSub(t3, t1);
                tmp = cc->EvalMult(t3, cc->MakeCKKSPackedPlaintext(weightsMatrix.at(19)));
                enc_out = cc->EvalAdd(tmp, enc_out);
                tmp = cc->EvalMult(t3, cc->MakeCKKSPackedPlaintext(weightsMatrix.at(20)));
                tmp = cc->EvalRotate(tmp, -1);
                enc_out = cc->EvalAdd(tmp, enc_out);
                tmp = cc->EvalMult(t3, cc->MakeCKKSPackedPlaintext(weightsMatrix.at(21)));
                tmp = cc->EvalRotate(tmp, -2);
                enc_out = cc->EvalAdd(tmp, enc_out);
                tmp = cc->EvalMult(t3, cc->MakeCKKSPackedPlaintext(weightsMatrix.at(22)));
                tmp = cc->EvalRotate(tmp, -3);
                enc_out = cc->EvalAdd(tmp, enc_out);

                auto t4 = cc->EvalMult(t2, t0);
                t4 = cc->EvalAdd(t4, t4);
                t4 = cc->EvalSub(t4, t0);
                tmp = cc->EvalMult(t4, cc->MakeCKKSPackedPlaintext(weightsMatrix.at(23)));
                enc_out = cc->EvalAdd(tmp, enc_out);
                tmp = cc->EvalMult(t4, cc->MakeCKKSPackedPlaintext(weightsMatrix.at(24)));
                tmp = cc->EvalRotate(tmp, -1);
                enc_out = cc->EvalAdd(tmp, enc_out);
                tmp = cc->EvalMult(t4, cc->MakeCKKSPackedPlaintext(weightsMatrix.at(25)));
                tmp = cc->EvalRotate(tmp, -2);
                enc_out = cc->EvalAdd(tmp, enc_out);
                tmp = cc->EvalMult(t4, cc->MakeCKKSPackedPlaintext(weightsMatrix.at(26)));
                tmp = cc->EvalRotate(tmp, -3);
                enc_out = cc->EvalAdd(tmp, enc_out);

                t0 = cc->EvalMult(t2, t1);
                t0 = cc->EvalAdd(t0, t0);
                t0 = cc->EvalSub(t0, m_inputC);
                tmp = cc->EvalMult(t0, cc->MakeCKKSPackedPlaintext(weightsMatrix.at(27)));
                enc_out = cc->EvalAdd(tmp, enc_out);
                tmp = cc->EvalMult(t0, cc->MakeCKKSPackedPlaintext(weightsMatrix.at(28)));
                tmp = cc->EvalRotate(tmp, -1);
                enc_out = cc->EvalAdd(tmp, enc_out);
                tmp = cc->EvalMult(t0, cc->MakeCKKSPackedPlaintext(weightsMatrix.at(29)));
                tmp = cc->EvalRotate(tmp, -2);
                enc_out = cc->EvalAdd(tmp, enc_out);
                tmp = cc->EvalMult(t0, cc->MakeCKKSPackedPlaintext(weightsMatrix.at(30)));
                tmp = cc->EvalRotate(tmp, -3);
                enc_out = cc->EvalAdd(tmp, enc_out);

                t1 = cc->EvalMult(t2, t2);
                t1 = cc->EvalAdd(t1, t1);
                t1 = cc->EvalSub(t1, 1);
                tmp = cc->EvalMult(t1, cc->MakeCKKSPackedPlaintext(weightsMatrix.at(31)));
                enc_out = cc->EvalAdd(tmp, enc_out);
                tmp = cc->EvalMult(t1, cc->MakeCKKSPackedPlaintext(weightsMatrix.at(32)));
                tmp = cc->EvalRotate(tmp, -1);
                enc_out = cc->EvalAdd(tmp, enc_out);
                tmp = cc->EvalMult(t1, cc->MakeCKKSPackedPlaintext(weightsMatrix.at(33)));
                tmp = cc->EvalRotate(tmp, -2);
                enc_out = cc->EvalAdd(tmp, enc_out);
                tmp = cc->EvalMult(t1, cc->MakeCKKSPackedPlaintext(weightsMatrix.at(34)));
                tmp = cc->EvalRotate(tmp, -3);
                enc_out = cc->EvalAdd(tmp, enc_out);
                tmp = cc->EvalRotate(enc_out, 1600);
                enc_out = cc->EvalAdd(enc_out, tmp);
                tmp = cc->EvalRotate(enc_out, 800);
                enc_out = cc->EvalAdd(enc_out, tmp);
                tmp = cc->EvalRotate(enc_out, 400);
                enc_out = cc->EvalAdd(enc_out, tmp);
                tmp = cc->EvalRotate(enc_out, 200);
                enc_out = cc->EvalAdd(enc_out, tmp);
                tmp = cc->EvalRotate(enc_out, 100);
                enc_out = cc->EvalAdd(enc_out, tmp);
                tmp = cc->EvalRotate(enc_out, 50);
                enc_out = cc->EvalAdd(enc_out, tmp);
                auto enc_out1 = cc->EvalRotate(enc_out, 40);
                tmp = cc->EvalRotate(enc_out, 20);
                enc_out = cc->EvalAdd(enc_out, tmp);
                tmp = cc->EvalRotate(enc_out, 10);
                enc_out = cc->EvalAdd(enc_out, tmp);
                enc_out = cc->EvalAdd(enc_out, enc_out1);

                return Ciphertext(std::move(enc_out));
            }

        private:
        };
    }  // namespace ckks
}  // namespace polycircuit
