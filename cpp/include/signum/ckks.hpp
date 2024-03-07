#include "openfhe.h"

// header files needed for serialization
#include "ciphertext-ser.h"
#include "cryptocontext-ser.h"
#include "key/key-ser.h"
#include "scheme/ckksrns/ckksrns-ser.h"

using namespace lbcrypto;

class SignumCKKS
{
    CryptoContext<DCRTPoly> m_cc;
    PublicKey<DCRTPoly> m_PublicKey;
    Ciphertext<DCRTPoly> m_InputC;
    Ciphertext<DCRTPoly> m_OutputC;
    std::string m_PubKeyLocation;
    std::string m_MultKeyLocation;
    std::string m_RotKeyLocation;
    std::string m_CCLocation;
    std::string m_InputLocation;
    std::string m_OutputLocation;

public:
    SignumCKKS(std::string ccLocation, std::string pubKeyLocation, std::string multKeyLocation,
               std::string rotKeyLocation,
               std::string inputLocation,
               std::string outputLocation);

    void initCC();

    void eval();

    void deserializeOutput();
};