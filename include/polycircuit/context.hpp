#pragma once

#include <openfhe/pke/constants-defs.h>
#include <openfhe/pke/cryptocontext.h>
#include <openfhe/pke/openfhe.h>
#include <openfhe/pke/scheme/ckksrns/gen-cryptocontext-ckksrns-params.h>

namespace polycircuit
{

using CKKSCryptoContext = lbcrypto::CryptoContext<lbcrypto::DCRTPoly>;
using CKKSCiphertext = lbcrypto::Ciphertext<lbcrypto::DCRTPoly>;
using CKKSSecretKey = lbcrypto::PrivateKey<lbcrypto::DCRTPoly>;
using CKKSPublicKey = lbcrypto::PublicKey<lbcrypto::DCRTPoly>;


struct CKKSContext {
    lbcrypto::KeyPair<lbcrypto::DCRTPoly> keys;
    lbcrypto::CryptoContext<lbcrypto::DCRTPoly> cc;

    CKKSCiphertext encrypt(const std::vector<double>& plaintext)
    {
        lbcrypto::Plaintext ptxt = cc->MakeCKKSPackedPlaintext(plaintext);
        auto c = cc->Encrypt(ptxt, keys.publicKey);

        return c;
    }

    std::vector<double> decrypt(const CKKSCiphertext& ciphertext, const CKKSSecretKey& sk, unsigned int slots)
    {
        lbcrypto::Plaintext ptxt;
        cc->Decrypt(ciphertext, sk, &ptxt);
        ptxt->SetLength(slots);

        return ptxt->GetRealPackedValue();
    }
};

struct CKKSParams {
    lbcrypto::CCParams<lbcrypto::CryptoContextCKKSRNS> params;
    std::vector<int> rotationKeys;
};

CKKSContext createCKKSContext(const CKKSParams& params)
{
    auto cc = lbcrypto::GenCryptoContext(params.params);

    cc->Enable(lbcrypto::PKESchemeFeature::PKE);
    cc->Enable(lbcrypto::PKESchemeFeature::KEYSWITCH);
    cc->Enable(lbcrypto::PKESchemeFeature::LEVELEDSHE);
    cc->Enable(lbcrypto::PKESchemeFeature::ADVANCEDSHE);
    cc->Enable(lbcrypto::PKESchemeFeature::FHE);

    auto keys = cc->KeyGen();
    cc->EvalMultKeyGen(keys.secretKey);
    cc->EvalRotateKeyGen(keys.secretKey, params.rotationKeys);

    return {keys, cc};
}

}