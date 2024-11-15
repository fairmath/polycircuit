#include <iostream>
#include <sstream>

#include "polycircuit/component/CIFAR10ImageClassification/CIFAR10ImageClassification.hpp"

#include <openfhe/pke/constants-defs.h>
#include <openfhe/pke/cryptocontext.h>
#include <openfhe/pke/openfhe.h>
#include <openfhe/pke/scheme/ckksrns/gen-cryptocontext-ckksrns-params.h>

#include <boost/program_options.hpp>

namespace po = boost::program_options;

/*
minimim required CKKS params
{
  "indexes_for_rotation_key": [-3, -2, -1, 10, 20, 40, 50, 100, 200, 400, 800, 1600],
  "mult_depth": 5,
  "ring_dimension": 8192,
  "scale_mod_size": 25,
  "first_mod_size": 30,
  "batch_size": 4096,
  "enable_bootstrapping": false,
  "levels_available_after_bootstrap": 0,
  "level_budget": [0,0]
}
*/
int main(int argc, char *argv[]) try
{
    po::options_description desc("Allowed parameters");
    desc.add_options()
    ("help,h", "produce help message")
    ("input", po::value<std::string>(), "path to space separated values corresponding to rgb color scheme");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help"))
    {
        std::cout << desc << '\n';
        return EXIT_SUCCESS;
    }
    if (!vm.count("input"))
    {
        throw std::runtime_error("Input file(--input) is not specified.");
    }

    lbcrypto::CCParams<lbcrypto::CryptoContextCKKSRNS> parameters;
    parameters.SetMultiplicativeDepth(5);
    parameters.SetScalingModSize(25);
    parameters.SetFirstModSize(30);
    parameters.SetScalingTechnique(lbcrypto::ScalingTechnique::FLEXIBLEAUTO);
    parameters.SetBatchSize(4096);
    parameters.SetRingDim(8192);

    lbcrypto::CryptoContext<lbcrypto::DCRTPoly> cc = lbcrypto::GenCryptoContext(parameters);

    std::cout << "CKKS scheme is using ring dimension " << cc->GetRingDimension() << std::endl;

    cc->Enable(lbcrypto::PKESchemeFeature::PKE);
    cc->Enable(lbcrypto::PKESchemeFeature::KEYSWITCH);
    cc->Enable(lbcrypto::PKESchemeFeature::LEVELEDSHE);
    cc->Enable(lbcrypto::PKESchemeFeature::ADVANCEDSHE);
    cc->Enable(lbcrypto::PKESchemeFeature::FHE);

    auto keys = cc->KeyGen();
    cc->EvalMultKeyGen(keys.secretKey);
    cc->EvalRotateKeyGen(
        keys.secretKey, 
       {-3, -2, -1, 10, 20, 40, 50, 100, 200, 400, 800, 1600}
    );

    std::cout << "mul and automorphism keys are gnerated " << std::endl;

    std::ifstream ifs(vm["input"].as<const std::string&>(), std::ios::in);
    if (!ifs.is_open())
    {
        throw std::runtime_error("Unable to read input file.");
    }
    std::vector<double> in{
        std::istream_iterator<double>{ifs},
        std::istream_iterator<double>{}
    };

    std::cout << "picture is loaded, size = " << in.size() << std::endl;

    lbcrypto::Plaintext ptxt = cc->MakeCKKSPackedPlaintext(in);
    auto c = cc->Encrypt(ptxt, keys.publicKey);

    std::cout << "encrypt data " << std::endl;

    auto result = std::get<lbcrypto::Ciphertext<lbcrypto::DCRTPoly>>(
        polycircuit::CIFAR10ImageClassification<lbcrypto::DCRTPoly>(cc, std::move(c)).evaluate()
    );

    std::cout << "solved CIFAR problem " << std::endl ;

    lbcrypto::Plaintext rptx;
    cc->Decrypt(result, keys.secretKey, &rptx);
    rptx->SetLength(10); // 10 classes of image

    std::cout << "decrypted result" << std::endl ;

    auto classes = rptx->GetRealPackedValue();
    std::for_each(classes.begin(), classes.end(), [](double e){ std::cout << e << " "; });
    std::cout << std::endl;
    
    return EXIT_SUCCESS;
}
catch (const po::error& ex)
{
    std::cerr << ex.what() << std::endl;
    return EXIT_FAILURE;
}
catch (const std::exception& ex)
{
    std::cerr << ex.what() << std::endl;
    return EXIT_FAILURE;
}
catch (...)
{
    std::cerr << "An unknown exception was thrown." << std::endl;
    return EXIT_FAILURE;
}

