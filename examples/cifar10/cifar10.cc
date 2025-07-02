#include <iostream>
#include <sstream>
#include <fstream>

#include "polycircuit/component/ml/cifar10/cifar10.hpp"

#include <openfhe/pke/constants-defs.h>
#include <openfhe/pke/encoding/plaintext-fwd.h>
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

    auto context = polycircuit::createCKKSContext(polycircuit::cifar10DefaultParams());
    auto keys = context.keys;
    auto cifar10 = polycircuit::Cifar10(context.cc, keys.publicKey);

    auto c = context.encrypt(in);

    std::cout << "encrypt data " << std::endl;

    polycircuit::CKKSArguments args(std::vector<polycircuit::CKKSCiphertext>{c});
    auto result = cifar10.evaluate(args);

    std::cout << "solved CIFAR problem " << std::endl ;

    auto classes = context.decrypt(result, keys.secretKey, 10);
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

