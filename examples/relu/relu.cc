#include "polycircuit/component/func/relu/relu.hpp"

#include <openfhe/pke/cryptocontext-ser.h>
#include <boost/program_options.hpp>
#include <iostream>

namespace po = boost::program_options;

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
        throw std::runtime_error("Specify input array: --input=\"0.1, -0.1, 0\"");
    }

    std::stringstream ss(vm["input"].as<std::string>());
    std::vector<double> in;

    std::transform(std::istream_iterator<std::string>(ss), {}, std::back_inserter(in),
        [](const std::string& s) {
            return std::stod(s);
        });

    std::cout << "read array: " << in.size() << std::endl;

    auto context = polycircuit::createCKKSContext(polycircuit::reluDefaultParams());
    auto keys = context.keys;
    auto relu = polycircuit::ReLU(context.cc, keys.publicKey);

    auto c = context.encrypt(in);

    std::cout << "encrypt data " << std::endl;

    polycircuit::CKKSArguments args(std::vector<polycircuit::CKKSCiphertext>{c});
    auto result = relu.evaluate(args);

    std::cout << "solved" << std::endl ;

    auto classes = context.decrypt(result, keys.secretKey, in.size());
    std::cout << "decrypted result" << std::endl ;
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

