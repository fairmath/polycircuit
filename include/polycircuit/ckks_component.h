#pragma once

#include "openfhe/pke/openfhe.h"
#include "icomponent.hpp"
#include <variant>

namespace polycircuit {

class CKKSComponent : public IComponent {
    lbcrypto::CCParams<lbcrypto::CryptoContextCKKSRNS> m_parameters;
    lbcrypto::CryptoContext<lbcrypto::DCRTPoly> m_cc;
    lbcrypto::KeyPair<lbcrypto::DCRTPoly> m_keys;
    std::vector<lbcrypto::Ciphertext<lbcrypto::DCRTPoly>> m_inputs;
    std::vector<int> m_rotateKeyIndexes;

    int m_batchSize;
    int m_ringDim;
    int m_scaleModSize;
    int m_firstModSize;
    int m_multDepth;

public:
    CKKSComponent();
    void GenerateCryptoContext();
    void KeyGen();

    void SetCryptoContext(lbcrypto::CryptoContext<lbcrypto::DCRTPoly> cc);
    void SetKeys(lbcrypto::KeyPair<lbcrypto::DCRTPoly> keys);
    void SetRotateKeyIndexes(std::vector<int> rotateKeyIndexes);
    void SetMultDepth(int multDepth);
    void SetRingDimension(int ringDim);
    void SetScaleModSize(int scaleModSize);
    void SetFirstModSize(int firstModSize);
    void SetBatchSize(int batchSize);

    void AddCipher(lbcrypto::Ciphertext<lbcrypto::DCRTPoly>);
    void EncryptAndAddCipher(std::vector<double> input);

    lbcrypto::CryptoContext<lbcrypto::DCRTPoly> GetCryptoContext() const;
    lbcrypto::KeyPair<lbcrypto::DCRTPoly> GetKeys() const;
    std::vector<lbcrypto::Ciphertext<lbcrypto::DCRTPoly>>& GetInputs();

    lbcrypto::Plaintext Decrypt(int index = 0);

    virtual ~CKKSComponent() = default;
};
}  // namespace polycircuit
