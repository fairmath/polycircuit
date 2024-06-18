#include "polycircuit/component/Signum.hpp"

#include <boost/program_options.hpp>

namespace po = boost::program_options;

int main(int argc, char *argv[]) try
{
    po::options_description desc("Allowed parameters");
    desc.add_options()
    ("help,h", "produce help message")
    ("cryptocontext_location", po::value<std::string>(), "set cryptocontext location")
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
        std::exit(EXIT_FAILURE);
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
        boost::any_cast<const std::string&>(vm["cryptocontext_location"].value()), cc, lbcrypto::SerType::BINARY))
    {
        std::cerr << "Could not deserialize cryptocontext." << std::endl;
        return EXIT_FAILURE;
    }

    lbcrypto::Ciphertext<lbcrypto::DCRTPoly> inputC;
    if (!lbcrypto::Serial::DeserializeFromFile(
        boost::any_cast<const std::string&>(vm["input_ciphertext_location"].value()), inputC, lbcrypto::SerType::BINARY))
    {
        std::cerr << "Could not deserialize ciphertext." << std::endl;
        return EXIT_FAILURE;
    }

    if (!lbcrypto::Serial::SerializeToFile(
        boost::any_cast<const std::string&>(vm["output_ciphertext_location"].value()),
        std::move(std::get<lbcrypto::Ciphertext<lbcrypto::DCRTPoly>>(
            polycircuit::Signum<lbcrypto::DCRTPoly>(std::move(cc), std::move(inputC)).evaluate())),
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

