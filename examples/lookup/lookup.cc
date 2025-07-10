#include "polycircuit/component/algo/lookup/lookup.hpp"

#include <openfhe/pke/cryptocontext-ser.h>
#include <boost/program_options.hpp>
#include <iostream>

namespace po = boost::program_options;

int main(int argc, char *argv[]) try
{
    po::options_description desc("Allowed parameters");
    desc.add_options()
    ("help,h", "produce help message")
    ("index", po::value<uint16_t>(), "set index value")
    ("array", po::value<std::string>(), "set array space separated values: \"1 2 3 4 5 6 7 8 9\"");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help"))
    {
        std::cout << desc << '\n';
        return EXIT_SUCCESS;
    }
    if (!vm.count("index") && !vm.count("array"))
    {
        throw std::runtime_error("Specify input args, example: --index=1 --array=\"0.1, -0.1, 0 2\"");
    }

    std::stringstream array(vm["array"].as<std::string>());
    std::vector<double> inArray;

    std::transform(std::istream_iterator<std::string>(array), {}, std::back_inserter(inArray),
        [](const std::string& s) {
            return std::stod(s);
        });

    for (auto i = inArray.size(); i < 2048; ++i) {
        inArray.push_back(0.0);
    }

    std::cout << "read array: " << inArray.size() << " elements" << std::endl;

    std::vector<double> inIndex(2048, 0);
    inIndex[0] = static_cast<double>(vm["index"].as<uint16_t>());

    auto params = polycircuit::algo::lookupDefaultParams();
    params.params.SetMultiplicativeDepth(17);
    params.params.SetBatchSize(2048);
    params.params.SetScalingModSize(55);
    params.params.SetFirstModSize(60);
    params.params.SetScalingTechnique(lbcrypto::ScalingTechnique::FLEXIBLEAUTO);
    params.params.SetRingDim(131072);
    params.rotationKeys = std::vector<int32_t>{1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, -2047};

    auto context = polycircuit::createCKKSContext(params);
    auto keys = context.keys;

    auto mul = polycircuit::algo::LookupTable(context.cc, keys.publicKey);

    auto c1 = context.encrypt(inArray);
    auto c2 = context.encrypt(inIndex);

    std::cout << "encrypt data " << std::endl;

    polycircuit::CKKSArguments args(std::vector<polycircuit::CKKSCiphertext>{c1, c2});
    auto result = mul.evaluate(args);

    std::cout << "solved" << std::endl ;

    auto decrypted = context.decrypt(result, keys.secretKey, 1);
    std::cout << "decrypted result" << std::endl ;
    std::for_each(decrypted.begin(), decrypted.end(), [](double e){ std::cout << e << " "; });
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

