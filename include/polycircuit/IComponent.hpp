#pragma once
#include "polycircuit/context.hpp"

#include "openfhe/core/lattice/hal/lat-backend.h"
#include "openfhe/pke/ciphertext-fwd.h"
#include "openfhe/pke/key/privatekey-fwd.h"
#include "openfhe/pke/cryptocontext.h"

#include <variant>

namespace polycircuit
{

using Ciphertext = std::variant<
    lbcrypto::Ciphertext<lbcrypto::DCRTPoly>,
    lbcrypto::Ciphertext<lbcrypto::Poly>,
    lbcrypto::Ciphertext<lbcrypto::NativePoly>>;

class IComponent
{
public:
    virtual ~IComponent() = default;
    virtual Ciphertext evaluate() = 0;
};

class CKKSArguments
{
public:
    explicit CKKSArguments(std::vector<CKKSCiphertext> ciphertexts)
        : m_ciphertexts(std::move(ciphertexts))
    { }

    CKKSArguments(std::vector<CKKSCiphertext> ciphertexts, std::vector<double> values)
        : m_ciphertexts(std::move(ciphertexts))
        , m_values(std::move(values))
    { }

    std::vector<CKKSCiphertext> ciphertexts() const { return m_ciphertexts; }
    std::vector<double> values() const { return m_values; }

    virtual ~CKKSArguments() = default;

private:
    std::vector<CKKSCiphertext> m_ciphertexts;
    std::vector<double> m_values;
};

class CKKSComponent
{
public:
    explicit CKKSComponent(CKKSCryptoContext&& context, CKKSPublicKey&& publicKey)
        : cc_(std::move(context))
        , pk_(std::move(publicKey))
    { }

    explicit CKKSComponent(const CKKSCryptoContext& context, const CKKSPublicKey& publicKey)
        : cc_(context)
        , pk_(publicKey)
    { }

    CKKSCiphertext evaluate(const CKKSArguments& args) {
        return eval(cc_, pk_, args);
    }

    virtual CKKSCiphertext eval(CKKSCryptoContext cc, CKKSPublicKey publicKey, const CKKSArguments& args) = 0;

private:
    CKKSCryptoContext cc_;
    CKKSPublicKey pk_;
};


} // polycircuit


