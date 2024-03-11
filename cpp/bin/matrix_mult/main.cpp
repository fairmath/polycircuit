#include <iostream>
#include <fstream>
#include <string>
#include "matrix_mult/ckks.hpp"

using namespace lbcrypto;

int main(int argc, char *argv[]) {
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

    MatrixMultCKKS matrixMultCKKS(ccLocation, pubKeyLocation, multKeyLocation, rotKeyLocation, matrixALocation,
                                  matrixBLocation,
                                  outputLocation);
    matrixMultCKKS.eval();
    matrixMultCKKS.deserializeOutput();
    return 0;
}
