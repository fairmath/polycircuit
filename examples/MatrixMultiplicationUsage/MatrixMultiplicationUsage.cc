#include "polycircuit/component/MatrixMultiplication/MatrixMultiplication.hpp"

#include <boost/program_options.hpp>

namespace po = boost::program_options;

int main(int argc, char *argv[]) try
{
    po::options_description desc("Allowed parameters");
    desc.add_options()
    ("help,h", "produce help message")
    ("size", po::value<int>(), "set square matrix size")
    ("cryptocontext_location", po::value<std::string>(), "set cryptocontext location")
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
        std::cerr << "Matrix size is not specified." << std::endl;
        return EXIT_FAILURE;
    }
    if (!vm.count("cryptocontext_location"))
    {
        std::cerr << "Cryptocontext location is not specified." << std::endl;
        return EXIT_FAILURE;
    }
    if (!vm.count("a_input_ciphertext_location"))
    {
        std::cerr << "Input ciphertext location of \"a\" matrix is not specified." << std::endl;
        return EXIT_FAILURE;
    }
    if (!vm.count("b_input_ciphertext_location"))
    {
        std::cerr << "Input ciphertext location of \"b\" matrix is not specified." << std::endl;
        return EXIT_FAILURE;
    }
    if (!vm.count("output_ciphertext_location"))
    {
        std::cerr << "Output ciphertext location is not specified." << std::endl;
        return EXIT_FAILURE;
    }

    lbcrypto::CryptoContext<lbcrypto::DCRTPoly> cc;
    if (!lbcrypto::Serial::DeserializeFromFile(
        boost::any_cast<const std::string&>(vm["cryptocontext_location"].value()), cc, lbcrypto::SerType::BINARY))
    {
        std::cerr << "Could not deserialize cryptocontext." << std::endl;
        return EXIT_FAILURE;
    }

    lbcrypto::Ciphertext<lbcrypto::DCRTPoly> aMatrixInputC;
    if (!lbcrypto::Serial::DeserializeFromFile(
        boost::any_cast<const std::string&>(vm["a_input_ciphertext_location"].value()), aMatrixInputC, lbcrypto::SerType::BINARY))
    {
        std::cerr << "Could not deserialize ciphertext." << std::endl;
        return EXIT_FAILURE;
    }

    lbcrypto::Ciphertext<lbcrypto::DCRTPoly> bMatrixInputC;
    if (!lbcrypto::Serial::DeserializeFromFile(
        boost::any_cast<const std::string&>(vm["b_input_ciphertext_location"].value()), bMatrixInputC, lbcrypto::SerType::BINARY))
    {
        std::cerr << "Could not deserialize ciphertext." << std::endl;
        return EXIT_FAILURE;
    }

    if (!lbcrypto::Serial::SerializeToFile(
        boost::any_cast<const std::string&>(vm["output_ciphertext_location"].value()),
        std::move(std::get<lbcrypto::Ciphertext<lbcrypto::DCRTPoly>>(
            polycircuit::MatrixMultiplication<lbcrypto::DCRTPoly>(
                boost::any_cast<int>(vm["size"].value()),
                std::move(cc),
                std::move(aMatrixInputC),
                std::move(bMatrixInputC)).evaluate())),
        lbcrypto::SerType::BINARY))
    {
        std::cerr << "Could not serialize ciphertext." << std::endl;
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

