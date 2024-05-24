#include <iostream>
#include <fstream>
#include <string>
#include "matrix_mult/ckks.hpp"

using namespace lbcrypto;

int main(int argc, char *argv[]) {
    using component_type = polycircuit::matrix_mult<64, CKKSRNS_SCHEME, DCRTPoly>;

    std::string pubKeyLocation;
    std::string multKeyLocation;
    std::string rotKeyLocation;
    std::string ccLocation;
    std::string matrixALocation;
    std::string matrixBLocation;
    std::string outputLocation;

    for (int i = 1; i < argc; i += 2) {
        std::string arg = argv[i];
        if (arg == "--key_pub") {
            pubKeyLocation = argv[i + 1];
        } else if (arg == "--key_mult") {
            multKeyLocation = argv[i + 1];
        } else if (arg == "--key_rot") {
            rotKeyLocation = argv[i + 1];
        } else if (arg == "--cc") {
            ccLocation = argv[i + 1];
        } else if (arg == "--matrix_a") {
            matrixALocation = argv[i + 1];
        } else if (arg == "--matrix_b") {
            matrixBLocation = argv[i + 1];
        } else if (arg == "--output") {
            outputLocation = argv[i + 1];
        }
    }

    CryptoContext<DCRTPoly> m_cc;
    PublicKey<DCRTPoly> m_PublicKey;
    Ciphertext<DCRTPoly> m_MatrixAC;
    Ciphertext<DCRTPoly> m_MatrixBC;
    
    if (!Serial::DeserializeFromFile(ccLocation, m_cc, SerType::BINARY)) {
        std::cerr << "Could not deserialize cryptocontext file" << std::endl;
        std::exit(1);
    }

    if (!Serial::DeserializeFromFile(pubKeyLocation, m_PublicKey, SerType::BINARY)) {
        std::cerr << "Could not deserialize public key file" << std::endl;
        std::exit(1);
    }

    std::ifstream multKeyIStream(multKeyLocation, std::ios::in | std::ios::binary);
    if (!multKeyIStream.is_open()) {
        std::exit(1);
    }

    if (!m_cc->DeserializeEvalMultKey(multKeyIStream, SerType::BINARY)) {
        std::cerr << "Could not deserialize rot key file" << std::endl;
        std::exit(1);
    }

    std::ifstream rotKeyIStream(rotKeyLocation, std::ios::in | std::ios::binary);
    if (!rotKeyIStream.is_open()) {
        std::exit(1);
    }

    if (!m_cc->DeserializeEvalAutomorphismKey(rotKeyIStream, SerType::BINARY)) {
        std::cerr << "Could not deserialize eval rot key file" << std::endl;
        std::exit(1);
    }

    if (!Serial::DeserializeFromFile(matrixALocation, m_MatrixAC, SerType::BINARY)) {
        std::cerr << "Could not deserialize Matrix A cipher" << std::endl;
        std::exit(1);
    }

    if (!Serial::DeserializeFromFile(matrixBLocation, m_MatrixBC, SerType::BINARY)) {
        std::cerr << "Could not deserialize Matrix B cipher" << std::endl;
        std::exit(1);
    }

    typename component_type::result_type OutputC =
        polycircuit::evaluate<component_type>(m_cc, m_PublicKey,
            {m_MatrixAC, m_MatrixBC});

    if (!Serial::SerializeToFile(outputLocation, OutputC, SerType::BINARY)) {
        std::cerr << " Error writing ciphertext 1" << std::endl;
    }
    return 0;
}
