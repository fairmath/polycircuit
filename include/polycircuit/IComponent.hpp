#pragma once

#include "openfhe/pke/openfhe.h"
#include <variant>

namespace polycircuit {
using Ciphertext = std::variant<lbcrypto::Ciphertext<lbcrypto::DCRTPoly>, lbcrypto::Ciphertext<lbcrypto::Poly>,
                                lbcrypto::Ciphertext<lbcrypto::NativePoly>>;

class IComponent {
public:
    virtual ~IComponent()         = default;
    virtual Ciphertext evaluate() = 0;
};

}  // namespace polycircuit
