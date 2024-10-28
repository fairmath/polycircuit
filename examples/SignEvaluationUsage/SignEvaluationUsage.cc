#include "polycircuit/component/SignEvaluation/SignEvaluation.hpp"

#include "openfhe/pke/cryptocontext-ser.h"

#include <boost/program_options.hpp>

namespace po = boost::program_options;

int main(int argc, char *argv[]) try
{
    po::options_description desc("Allowed parameters");
    desc.add_options()
    ("help,h", "produce help message")
    ("cryptocontext_location", po::value<std::string>(), "set cryptocontext location")
    ("mult_key_location", po::value<std::string>(), "set mult_key location")
    ("input_ciphertext_location", po::value<std::string>(), "set input ciphertext location")
    ("output_ciphertext_location", po::value<std::string>(), "set output ciphertext location");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help"))
    {
        std::cout << desc << '\n';
        return EXIT_SUCCESS;
    }
    if (!vm.count("cryptocontext_location"))
    {
        std::cerr << "Cryptocontext location is not specified." << std::endl;
        return EXIT_FAILURE;
    }
    if (!vm.count("mult_key_location"))
    {
        std::cerr << "mult_key location is not specified." << std::endl;
        return EXIT_FAILURE;
    }
    if (!vm.count("input_ciphertext_location"))
    {
        std::cerr << "Input ciphertext location is not specified." << std::endl;
        return EXIT_FAILURE;
    }
    if (!vm.count("output_ciphertext_location"))
    {
        std::cerr << "Output ciphertext location is not specified." << std::endl;
        return EXIT_FAILURE;
    }

    lbcrypto::CryptoContext<lbcrypto::DCRTPoly> cc;
    if (!lbcrypto::Serial::DeserializeFromFile(
        vm["cryptocontext_location"].as<const std::string&>(), cc, lbcrypto::SerType::BINARY))
    {
        std::cerr << "Unable to deserialize cryptocontext." << std::endl;
        return EXIT_FAILURE;
    }

    std::ifstream ifsMultKey(vm["mult_key_location"].as<const std::string&>(), std::ios::in | std::ios::binary);
    if (!ifsMultKey.is_open())
    {
        std::cerr << "Unable to read mult_key." << std::endl;
        return EXIT_FAILURE;
    }
    if (!cc->DeserializeEvalMultKey(ifsMultKey, lbcrypto::SerType::BINARY))
    {
        std::cerr << "Unable to deserialize mult_key." << std::endl;
        return EXIT_FAILURE;
    }
    ifsMultKey.close();

    lbcrypto::Ciphertext<lbcrypto::DCRTPoly> inputC;
    if (!lbcrypto::Serial::DeserializeFromFile(
        vm["input_ciphertext_location"].as<const std::string&>(), inputC, lbcrypto::SerType::BINARY))
    {
        std::cerr << "Unable to deserialize ciphertext." << std::endl;
        return EXIT_FAILURE;
    }

    if (!lbcrypto::Serial::SerializeToFile(
        vm["output_ciphertext_location"].as<const std::string&>(),
        std::move(std::get<lbcrypto::Ciphertext<lbcrypto::DCRTPoly>>(
            polycircuit::SignEvaluation<lbcrypto::DCRTPoly>(std::move(cc), std::move(inputC)).evaluate())),
        lbcrypto::SerType::BINARY))
    {
        std::cerr << "Unable to serialize ciphertext." << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
catch (const po::error& ex)
{
    std::cerr << ex.what() << '\n';
    return EXIT_FAILURE;
}
catch (const std::exception& ex)
{
    std::cerr << ex.what() << '\n';
    return EXIT_FAILURE;
}
catch (...)
{
    std::cerr << "Unknown exception.\n";
    return EXIT_FAILURE;
}

