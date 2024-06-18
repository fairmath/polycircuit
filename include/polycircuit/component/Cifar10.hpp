#pragma once

#include "polycircuit/IComponent.hpp"

#include "openfhe/pke/cryptocontext.h"

namespace polycircuit
{

template <typename ElementType>
class Cifar10 final : public IComponent
{
public:
    explicit Cifar10(lbcrypto::CryptoContext<ElementType>&& cc,
                     lbcrypto::Ciphertext<ElementType>&& inputC)
        : m_cc(std::move(cc))
        , m_inputC(std::move(inputC))
    { }
    explicit Cifar10(const lbcrypto::CryptoContext<ElementType>& cc,
                     const lbcrypto::Ciphertext<ElementType>& inputC)
        : m_cc(cc)
        , m_inputC(inputC)
    { }
    Cifar10(const Cifar10&) = default;
    Cifar10(Cifar10&&) = default;
    Cifar10& operator=(const Cifar10&) = default;
    Cifar10& operator=(Cifar10&&) = default;
/*
    Ciphertext evaluate() override
    {
        // TODO:
    }
*/
private:
    lbcrypto::CryptoContext<ElementType> m_cc;
    lbcrypto::Ciphertext<ElementType> m_inputC;
};

} // polycircuit


