#include "polycircuit/ckks_component.h"

namespace polycircuit {

CKKSComponent::CKKSComponent() {}

void CKKSComponent::GenerateCryptoContext() {
    m_cc = lbcrypto::GenCryptoContext(m_parameters);
    m_cc->Enable(lbcrypto::PKESchemeFeature::PKE);
    m_cc->Enable(lbcrypto::PKESchemeFeature::KEYSWITCH);
    m_cc->Enable(lbcrypto::PKESchemeFeature::LEVELEDSHE);
    m_cc->Enable(lbcrypto::PKESchemeFeature::ADVANCEDSHE);
    m_cc->Enable(lbcrypto::PKESchemeFeature::FHE);
}

lbcrypto::CryptoContext<lbcrypto::DCRTPoly> CKKSComponent::GetCryptoContext() const {
    return m_cc;
}

lbcrypto::KeyPair<lbcrypto::DCRTPoly> CKKSComponent::GetKeys() const {
    return m_keys;
}

std::vector<lbcrypto::Ciphertext<lbcrypto::DCRTPoly>>& CKKSComponent::GetInputs() {
    return m_inputs;
}

void CKKSComponent::KeyGen() {
    m_keys = m_cc->KeyGen();
    if (!m_rotateKeyIndexes.empty())
        m_cc->EvalRotateKeyGen(m_keys.secretKey, m_rotateKeyIndexes);
}

void CKKSComponent::SetCryptoContext(lbcrypto::CryptoContext<lbcrypto::DCRTPoly> cc) {
    m_cc = cc;
}

void CKKSComponent::SetKeys(lbcrypto::KeyPair<lbcrypto::DCRTPoly> keys) {
    m_keys = keys;
}

lbcrypto::Plaintext CKKSComponent::Decrypt(int index) {
    lbcrypto::Plaintext ptx;
    m_cc->Decrypt(m_inputs[index], m_keys.secretKey, &ptx);
    return ptx;
}

void CKKSComponent::SetRotateKeyIndexes(std::vector<int> rotateKeyIndexes) {
    m_rotateKeyIndexes = rotateKeyIndexes;
}

void CKKSComponent::SetMultDepth(int multDepth) {
    m_multDepth = multDepth;
}

void CKKSComponent::SetRingDimension(int ringDim) {
    m_ringDim = ringDim;
}

void CKKSComponent::SetScaleModSize(int scaleModSize) {
    m_scaleModSize = scaleModSize;
}

void CKKSComponent::SetFirstModSize(int firstModSize) {
    m_firstModSize = firstModSize;
}

void CKKSComponent::SetBatchSize(int batchSize) {
    m_batchSize = batchSize;
}

void CKKSComponent::AddCipher(lbcrypto::Ciphertext<lbcrypto::DCRTPoly> cipher) {
    m_inputs.push_back(cipher);
}

void CKKSComponent::EncryptAndAddCipher(std::vector<double> input) {
    lbcrypto::Plaintext ptxt = m_cc->MakeCKKSPackedPlaintext(input);
    auto c                   = m_cc->Encrypt(ptxt, m_keys.publicKey);
    m_inputs.push_back(c);
}
}  // namespace polycircuit
