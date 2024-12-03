#include <iostream>
#include <sstream>

#include "polycircuit/cifar10/cifar10.hpp"

#include <openfhe/pke/constants-defs.h>
#include <openfhe/pke/cryptocontext.h>
#include <openfhe/pke/openfhe.h>
#include <openfhe/pke/scheme/ckksrns/gen-cryptocontext-ckksrns-params.h>

/*
minimim required CKKS params
{
  "indexes_for_rotation_key": [-3, -2, -1, 10, 20, 40, 50, 100, 200, 400, 800,
1600], "mult_depth": 5, "ring_dimension": 8192, "scale_mod_size": 25,
  "first_mod_size": 30,
  "batch_size": 4096,
  "enable_bootstrapping": false,
  "levels_available_after_bootstrap": 0,
  "level_budget": [0,0]
}
*/
int main(int argc, char *argv[]) try {
    polycircuit::ckks::Cifar10 cifar10Evaluator;

    cifar10Evaluator.GenerateCryptoContext();
    cifar10Evaluator.KeyGen();

    std::ifstream ifs(
            "/Users/gg/projects/fairmath/polycircuit_new/examples/"
            "Cifar10/images/class1.txt");
    if (!ifs.is_open()) {
        throw std::runtime_error("Unable to read input file.");
    }
    std::vector<double> in{std::istream_iterator<double>{ifs}, std::istream_iterator<double>{}};

    std::cout << "picture is loaded, size = " << in.size() << std::endl;

    cifar10Evaluator.EncryptAndAddCipher(in);

    std::cout << "encrypt data " << std::endl;

    auto result = std::get<lbcrypto::Ciphertext<lbcrypto::DCRTPoly>>(cifar10Evaluator.evaluate());

    std::cout << "solved CIFAR problem " << std::endl;

    lbcrypto::Plaintext rptx;
    cifar10Evaluator.GetCryptoContext()->Decrypt(result, cifar10Evaluator.GetKeys().secretKey, &rptx);
    rptx->SetLength(10);  // 10 classes of image

    std::cout << "decrypted result" << std::endl;

    auto classes = rptx->GetRealPackedValue();
    std::for_each(classes.begin(), classes.end(), [](double e) { std::cout << e << " "; });
    std::cout << std::endl;

    return EXIT_SUCCESS;
}

catch (const std::exception &ex) {
    std::cerr << ex.what() << std::endl;
    return EXIT_FAILURE;
}
catch (...) {
    std::cerr << "An unknown exception was thrown." << std::endl;
    return EXIT_FAILURE;
}
