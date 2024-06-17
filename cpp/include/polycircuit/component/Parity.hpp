#pragma once

#include "polycircuit/IComponent.hpp"

#include "openfhe/pke/cryptocontext.h"

namespace polycircuit
{

template <typename ElementType>
class Parity final : public IComponent
{
public:
    explicit Parity(lbcrypto::CryptoContext<ElementType>&& cc,
                    lbcrypto::Ciphertext<ElementType>&& inputC)
        : m_cc(std::move(cc))
        , m_inputC(std::move(inputC))
    { }
    explicit Parity(const lbcrypto::CryptoContext<ElementType>& cc,
                    const lbcrypto::Ciphertext<ElementType>& inputC)
        : m_cc(cc)
        , m_inputC(inputC)
    { }
    Parity(const Parity&) = default;
    Parity(Parity&&) = default;
    Parity& operator=(const Parity&) = default;
    Parity& operator=(Parity&&) = default;
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


