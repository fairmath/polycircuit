#pragma once

#include "polycircuit/IComponent.hpp"

#include "openfhe/pke/cryptocontext.h"

namespace polycircuit
{

template <typename ElementType>
class Relu final : public IComponent
{
public:
    explicit Relu(lbcrypto::CryptoContext<ElementType>&& cc,
                  lbcrypto::Ciphertext<ElementType>&& inputC)
        : m_cc(std::move(cc))
        , m_inputC(std::move(inputC))
    { }
    explicit Relu(const lbcrypto::CryptoContext<ElementType>& cc,
                  const lbcrypto::Ciphertext<ElementType>& inputC)
        : m_cc(cc)
        , m_inputC(inputC)
    { }
    Relu(const Relu&) = default;
    Relu(Relu&&) = default;
    Relu& operator=(const Relu&) = default;
    Relu& operator=(Relu&&) = default;
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

