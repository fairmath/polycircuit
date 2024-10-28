#include "polycircuit/component/MatrixMultiplication/MatrixMultiplication.hpp"

#include <openfhe/pke/cryptocontext-ser.h>

#include <boost/program_options.hpp>

namespace po = boost::program_options;

int main(int argc, char *argv[]) try
{
    po::options_description desc("Allowed parameters");
    desc.add_options()
    ("help,h", "produce help message")
    ("size", po::value<int>(), "set square matrix size")
    ("cryptocontext_location", po::value<std::string>(), "set cryptocontext location")
    ("mult_key_location", po::value<std::string>(), "set mult_key location")
    ("rotate_key_location", po::value<std::string>(), "set rotate_key location")
    ("a_input_ciphertext_location", po::value<std::string>(), "set input ciphertext location of \"a\"")
    ("b_input_ciphertext_location", po::value<std::string>(), "set input ciphertext location of \"b\"")
    ("output_ciphertext_location", po::value<std::string>(), "set output ciphertext location");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help"))
    {
        std::cout << desc << '\n';
        return EXIT_SUCCESS;
    }
    if (!vm.count("size"))
    {
        throw std::runtime_error("Matrix size is not specified.");
    }
    if (!vm.count("cryptocontext_location"))
    {
        throw std::runtime_error("Cryptocontext location is not specified.");
    }
    if (!vm.count("mult_key_location"))
    {
        throw std::runtime_error("mult_key location is not specified.");
    }
    if (!vm.count("rotate_key_location"))
    {
        throw std::runtime_error("rotate_key location is not specified.");
    }
    if (!vm.count("a_input_ciphertext_location"))
    {
        throw std::runtime_error("Input ciphertext location of \"a\" matrix is not specified.");
    }
    if (!vm.count("b_input_ciphertext_location"))
    {
        throw std::runtime_error("Input ciphertext location of \"b\" matrix is not specified.");
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

    std::ifstream ifsRotateKey(vm["rotate_key_location"].as<const std::string&>(), std::ios::in | std::ios::binary);
    if (!ifsRotateKey.is_open())
    {
        throw std::runtime_error("Unable to read rotate_key.");
    }
    if (!cc->DeserializeEvalAutomorphismKey(ifsRotateKey, lbcrypto::SerType::BINARY))
    {
        ifsRotateKey.close();
        throw std::runtime_error("Unable to deserialize rotate_key.");
    }
    ifsRotateKey.close();

    lbcrypto::Ciphertext<lbcrypto::DCRTPoly> aMatrixInputC;
    if (!lbcrypto::Serial::DeserializeFromFile(
        vm["a_input_ciphertext_location"].as<const std::string&>(), aMatrixInputC, lbcrypto::SerType::BINARY))
    {
        throw std::runtime_error("Unable to deserialize ciphertext.");
    }

    lbcrypto::Ciphertext<lbcrypto::DCRTPoly> bMatrixInputC;
    if (!lbcrypto::Serial::DeserializeFromFile(
        vm["b_input_ciphertext_location"].as<const std::string&>(), bMatrixInputC, lbcrypto::SerType::BINARY))
    {
        throw std::runtime_error("Unable to deserialize ciphertext.");
    }

    if (!lbcrypto::Serial::SerializeToFile(
        vm["output_ciphertext_location"].as<const std::string&>(),
        std::move(std::get<lbcrypto::Ciphertext<lbcrypto::DCRTPoly>>(
            polycircuit::MatrixMultiplication<lbcrypto::DCRTPoly>(
                vm["size"].as<int>(),
                std::move(cc),
                std::move(aMatrixInputC),
                std::move(bMatrixInputC)).evaluate())),
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

