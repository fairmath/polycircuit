#pragma once

#include "polycircuit/IComponent.hpp"

#include "openfhe/pke/cryptocontext.h"

namespace polycircuit
{

template <typename ElementType>
class Sigmoid final : public IComponent
{
public:
    explicit Sigmoid(lbcrypto::CryptoContext<ElementType>&& cc,
                     lbcrypto::Ciphertext<ElementType>&& inputC)
        : m_cc(std::move(cc))
        , m_inputC(std::move(inputC))
    { }
    explicit Sigmoid(const lbcrypto::CryptoContext<ElementType>& cc,
                     const lbcrypto::Ciphertext<ElementType>& inputC)
        : m_cc(cc)
        , m_inputC(inputC)
    { }
    Sigmoid(const Sigmoid&) = default;
    Sigmoid(Sigmoid&&) = default;
    Sigmoid& operator=(const Sigmoid&) = default;
    Sigmoid& operator=(Sigmoid&&) = default;

    Ciphertext evaluate() override
    {
        // These are only the top 59 values
        static std::vector<double> coeff_val({
            1,   0.6349347497444793,      0.0, -0.207226910968973,      0.0, 0.11926554318627501,
            0.0, -0.08013715047239724,    0.0, 0.05757992161586102,     0.0, -0.04280910730211544,
            0.0, 0.03243169753850008,     0.0, -0.024837099847818355,   0.0, 0.01914266900321147,
            0.0, -0.014810271063030017,   0.0, 0.011484876770357881,    0.0, -0.008918658212582378,
            0.0, 0.006931783590193362,    0.0, -0.00539036659534493,    0.0, 0.004193061697765822,
            0.0, -0.0032623449132251907,  0.0, 0.0025385228671728505,   0.0, -0.0019754432169236547,
            0.0, 0.001537332438236821,    0.0, -0.00119641841400218,    0.0, 0.0009311199764515702,
            0.0, -0.0007246568620069462,  0.0, 0.0005639769377601768,   0.0, -0.00043892561744415164,
            0.0, 0.00034160150542931166,  0.0, -0.00026585589670811906, 0.0, 0.00020690371892740422,
            0.0, -0.00016102095964053078, 0.0, 0.0001253092833997599});

        // These are the remaining values
        static std::vector<double> coeff_val3({
            -9.751288354472837e-05, 0.0, 7.587595780529826e-05,  0.0, -5.903178548385316e-05,  0.0,
            4.591638974071583e-05,  0.0, -3.570132724330664e-05, 0.0, 2.7741357204113592e-05,  0.0,
            -2.15336836799884e-05,  0.0, 1.668619567482025e-05,  0.0, -1.2892699205061372e-05, 0.0,
            9.91357617352584e-06,   0.0, -7.560648875376369e-06});

        m_cc->Enable(lbcrypto::PKESchemeFeature::ADVANCEDSHE);
        lbcrypto::Ciphertext<ElementType> outputC = m_cc->EvalChebyshevSeries(m_inputC, coeff_val, -25, 25);

        std::vector<lbcrypto::Ciphertext<ElementType>> t1(15);
        t1[0] = m_inputC;                       // 0
        t1[1] = m_cc->EvalMult(m_inputC, 0.04); // 1
        // T2=2x^2-1
        lbcrypto::Ciphertext<ElementType> prod;
        prod = m_cc->EvalMult(t1[1], t1[1]);
        t1[2] = m_cc->EvalSub(m_cc->EvalAdd(prod, prod), 1); // 2
        // T3=4x^3-3x
        prod = m_cc->EvalMult(m_cc->EvalMult(t1[0], 4 * std::pow(0.04, 3)), m_cc->EvalSquare(t1[0]));
        t1[3] = m_cc->EvalSub(prod, m_cc->EvalMult(t1[1], 3)); // 2
        // T4=2T_2^2-1
        prod = m_cc->EvalMult(t1[2], t1[2]);
        t1[4] = m_cc->EvalSub(m_cc->EvalAdd(prod, prod), 1); // 3
        // T5=2T_2T_3^2-T_1
        prod = m_cc->EvalMult(t1[2], t1[3]);
        t1[5] = m_cc->EvalSub(m_cc->EvalAdd(prod, prod), t1[1]); // 3
        // T6=2T_3T_3-1
        prod = m_cc->EvalMult(t1[3], t1[3]);
        t1[6] = m_cc->EvalSub(m_cc->EvalAdd(prod, prod), 1); // 3
        prod = m_cc->EvalMult(t1[3], t1[4]);
        t1[7] = m_cc->EvalSub(m_cc->EvalAdd(prod, prod), t1[1]); // 3
        // T8=2T_4^2-1
        prod = m_cc->EvalMult(t1[4], t1[4]);
        t1[8] = m_cc->EvalSub(m_cc->EvalAdd(prod, prod), 1); // 4
        // T9=2T_4T_5-T_1
        prod = m_cc->EvalMult(t1[4], t1[5]);
        t1[9] = m_cc->EvalSub(m_cc->EvalAdd(prod, prod), t1[1]); // 4
        // T10=2T_5T_5^2-1
        prod = m_cc->EvalMult(t1[5], t1[5]);
        t1[10] = m_cc->EvalSub(m_cc->EvalAdd(prod, prod), 1); // 4
        // T11=2T_5T_6^2-T_1
        prod = m_cc->EvalMult(t1[5], t1[6]);
        t1[11] = m_cc->EvalSub(m_cc->EvalAdd(prod, prod), t1[1]); // 4
        // T12=2T_6T_6^2-1
        prod = m_cc->EvalMult(t1[5], t1[5]);
        t1[12] = m_cc->EvalSub(m_cc->EvalAdd(prod, prod), 1); // 4
        // T13=2T_5T_6^2-T_1
        prod = m_cc->EvalMult(t1[5], t1[6]);
        t1[13] = m_cc->EvalSub(m_cc->EvalAdd(prod, prod), t1[1]); // 4
        // T14=2T_7T_7^2-1
        prod = m_cc->EvalMult(t1[7], t1[7]);
        t1[14] = m_cc->EvalSub(m_cc->EvalAdd(prod, prod), 1); // 4

        std::vector<lbcrypto::Ciphertext<ElementType>> t(64);
        const int l = 63;
        for (int i = 1; i <= 14; i++)
        {
            t[i] = t1[i];
        }
        for (int i = 15; i < l + 1; i++)
        {
            int j = i / 2;
            prod = m_cc->EvalMult(t[j], t[i - j]);
            t[i] = m_cc->EvalAdd(prod, prod);
            if (2 * j == i)
            {
                m_cc->EvalSubInPlace(t[i], 1);
            }
            else
            {
                m_cc->EvalSubInPlace(t[i], t[1]);
            }
        }

        lbcrypto::Ciphertext<ElementType> temp1, temp2, sum;
        double coeff;
        // T59

        for (int i = 0; i < 1; i++)
        {
            coeff = coeff_val3[i];
            temp1 = m_cc->EvalMult(m_cc->EvalMult(t[3 + i], coeff * 8), t[8]);
            temp2 = m_cc->EvalMult(t[5 - i], coeff * 4);
            temp1 = m_cc->EvalSub(temp1, temp2);
            temp1 = m_cc->EvalMult(temp1, t[16]);
            temp2 = m_cc->EvalMult(t[5 - i], coeff * 2);
            temp1 = m_cc->EvalSub(temp1, temp2);
            temp1 = m_cc->EvalMult(temp1, t[32]);
            temp2 = m_cc->EvalMult(t[5 - i], coeff);
            temp1 = m_cc->EvalSub(temp1, temp2);
            if (i == 0)
            {
                sum = temp1;
            }
            else
            {
                sum = m_cc->EvalAdd(sum, temp1);
            }
        }

        // T61
        for (int i = 0; i < 1; i++)
        {
            coeff = coeff_val3[2 + i]; //-coeff_val3[8+i];
            temp1 = m_cc->EvalMult(m_cc->EvalMult(t[1 + i], coeff * 16), t[4]);
            temp2 = m_cc->EvalMult(t[3 - i], coeff * 8);
            temp1 = m_cc->EvalSub(temp1, temp2);
            temp1 = m_cc->EvalMult(temp1, t[8]);
            temp2 = m_cc->EvalMult(t[3 - i], coeff * 4);
            temp1 = m_cc->EvalSub(temp1, temp2);
            temp1 = m_cc->EvalMult(temp1, t[16]);
            temp2 = m_cc->EvalMult(t[3 - i], coeff * 2);
            temp1 = m_cc->EvalSub(temp1, temp2);
            temp1 = m_cc->EvalMult(temp1, t[32]);
            temp2 = m_cc->EvalMult(t[3 - i], coeff);
            temp1 = m_cc->EvalSub(temp1, temp2);

            sum = m_cc->EvalAdd(sum, temp1);
        }

        // T63
        for (int i = 0; i < 1; i++)
        {
            coeff = coeff_val3[4 + i] - coeff_val3[6 + i];
            temp1 = m_cc->EvalMult(m_cc->EvalMult(t[1], coeff * 32), t[2]);
            temp2 = m_cc->EvalMult(t[1], coeff * 16);
            temp1 = m_cc->EvalSub(temp1, temp2);
            temp1 = m_cc->EvalMult(temp1, t[4]);
            temp2 = m_cc->EvalMult(t[1], coeff * 8);
            temp1 = m_cc->EvalSub(temp1, temp2);
            temp1 = m_cc->EvalMult(temp1, t[8]);
            temp2 = m_cc->EvalMult(t[1], coeff * 4);
            temp1 = m_cc->EvalSub(temp1, temp2);
            temp1 = m_cc->EvalMult(temp1, t[16]);
            temp2 = m_cc->EvalMult(t[1], coeff * 2);
            temp1 = m_cc->EvalSub(temp1, temp2);
            temp1 = m_cc->EvalMult(temp1, t[32]);
            temp2 = m_cc->EvalMult(t[1], coeff);
            temp1 = m_cc->EvalSub(temp1, temp2);

            sum = m_cc->EvalAdd(sum, temp1);
        }

        // T65
        coeff = coeff_val3[6];

        temp1 = m_cc->EvalMult(m_cc->EvalMult(m_inputC, (coeff * 32 * 4) / (std::pow(25, 3))), m_cc->EvalSquare(m_inputC));
        temp2 = m_cc->EvalMult(t[1], coeff * 64);
        temp1 = m_cc->EvalSub(temp1, temp2);
        temp1 = m_cc->EvalMult(temp1, t[2]);
        temp2 = m_cc->EvalMult(t[1], coeff * 32);
        temp1 = m_cc->EvalSub(temp1, temp2);
        temp1 = m_cc->EvalMult(temp1, t[4]);
        temp2 = m_cc->EvalMult(t[1], coeff * 16);
        temp1 = m_cc->EvalSub(temp1, temp2);
        temp1 = m_cc->EvalMult(temp1, t[8]);
        temp2 = m_cc->EvalMult(t[1], coeff * 8);
        temp1 = m_cc->EvalSub(temp1, temp2);
        temp1 = m_cc->EvalMult(temp1, t[16]);
        temp2 = m_cc->EvalMult(t[1], coeff * 4);
        temp1 = m_cc->EvalSub(temp1, temp2);
        temp1 = m_cc->EvalMult(temp1, t[32]);
        temp2 = m_cc->EvalMult(t[1], coeff * 2);
        temp1 = m_cc->EvalSub(temp1, temp2);

        sum = m_cc->EvalAdd(sum, temp1);

        // T67,69,71,73,75,77
        // T3=4x^3-3x
        for (int i = 0; i < 6; i++)
        {
            coeff = coeff_val3[8 + 2 * i];
            prod = m_cc->EvalMult(m_cc->EvalMult(m_inputC, 32 * coeff * 4 * std::pow(0.04, 3)),
                                  m_cc->EvalSquare(m_inputC));
            temp2 = m_cc->EvalSub(prod, m_cc->EvalMult(m_inputC, 3 * 32 * coeff * 0.04)); // 2
            temp1 = m_cc->EvalMult(temp2, t[2]);
            temp2 = m_cc->EvalMult(t[1], coeff * 16);
            temp1 = m_cc->EvalSub(temp1, temp2);
            temp1 = m_cc->EvalMult(temp1, t[4]);
            temp2 = m_cc->EvalMult(t[1], coeff * 8);
            temp1 = m_cc->EvalSub(temp1, temp2);
            temp1 = m_cc->EvalMult(temp1, t[8]);
            temp2 = m_cc->EvalMult(t[1], coeff * 4);
            temp1 = m_cc->EvalSub(temp1, temp2);
            temp1 = m_cc->EvalMult(temp1, t[16]);
            temp2 = m_cc->EvalMult(t[1], coeff * 2);
            temp1 = m_cc->EvalSub(temp1, temp2);
            temp1 = m_cc->EvalMult(temp1, t[34 + i * 2]);
            temp2 = m_cc->EvalMult(t[1 + i * 2], coeff);
            temp1 = m_cc->EvalSub(temp1, temp2);

            sum = m_cc->EvalAdd(sum, temp1);
        }

        outputC = m_cc->EvalAdd(outputC, sum);
        return Ciphertext(std::move(outputC));
    }

private:
    lbcrypto::CryptoContext<ElementType> m_cc;
    lbcrypto::Ciphertext<ElementType> m_inputC;
};

} // polycircuit

