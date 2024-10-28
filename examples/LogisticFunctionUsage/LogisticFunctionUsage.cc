#include "polycircuit/component/LogisticFunction/LogisticFunction.hpp"

#include <openfhe/pke/cryptocontext-ser.h>

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
        throw std::runtime_error("Cryptocontext location is not specified.");
    }
    if (!vm.count("mult_key_location"))
    {
        throw std::runtime_error("mult_key location is not specified.");
    }
    if (!vm.count("input_ciphertext_location"))
    {
        throw std::runtime_error("Input ciphertext location is not specified.");
    }
    if (!vm.count("output_ciphertext_location"))
    {
        throw std::runtime_error("Output ciphertext location is not specified.");
    }

    lbcrypto::CryptoContext<lbcrypto::DCRTPoly> cc;
    if (!lbcrypto::Serial::DeserializeFromFile(
        vm["cryptocontext_location"].as<const std::string&>(), cc, lbcrypto::SerType::BINARY))
    {
        throw std::runtime_error("Unable to deserialize cryptocontext.");
    }

    std::ifstream ifsMultKey(vm["mult_key_location"].as<const std::string&>(), std::ios::in | std::ios::binary);
    if (!ifsMultKey.is_open())
    {
        throw std::runtime_error("Unable to read mult_key.");
    }
    if (!cc->DeserializeEvalMultKey(ifsMultKey, lbcrypto::SerType::BINARY))
    {
        ifsMultKey.close();
        throw std::runtime_error("Unable to deserialize mult_key.");
    }
    ifsMultKey.close();

    lbcrypto::Ciphertext<lbcrypto::DCRTPoly> inputC;
    if (!lbcrypto::Serial::DeserializeFromFile(
        vm["input_ciphertext_location"].as<const std::string&>(), inputC, lbcrypto::SerType::BINARY))
    {
        throw std::runtime_error("Unable to deserialize ciphertext.");
    }

    if (!lbcrypto::Serial::SerializeToFile(
        vm["output_ciphertext_location"].as<const std::string&>(),
        std::move(std::get<lbcrypto::Ciphertext<lbcrypto::DCRTPoly>>(
            polycircuit::LogisticFunction<lbcrypto::DCRTPoly>(std::move(cc), std::move(inputC)).evaluate())),
        lbcrypto::SerType::BINARY))
    {
        throw std::runtime_error("Unable to serialize ciphertext.");
    }

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

