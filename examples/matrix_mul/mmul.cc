#include "polycircuit/component/algo/matrix/mul.hpp"

#include <openfhe/pke/cryptocontext-ser.h>
#include <boost/program_options.hpp>
#include <iostream>

namespace po = boost::program_options;
namespace polymatrix = polycircuit::algo::matrix;

int main(int argc, char *argv[]) try
{
    po::options_description desc("Allowed parameters");
    desc.add_options()
    ("help,h", "produce help message")
    ("msize", po::value<int>(), "set square matrix size")
    ("ma", po::value<std::string>(), "set matrix a space separated values: \"1 2 3 4 5 6 7 8 9\"")
    ("mb", po::value<std::string>(), "set matrix b space separated values: \"1 2 3 4 5 6 7 8 9\"");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help"))
    {
        std::cout << desc << '\n';
        return EXIT_SUCCESS;
    }
    if (!vm.count("ma") && !vm.count("mb") && !vm.count("msize"))
    {
        throw std::runtime_error("Specify input args, example: --ma=\"0.1, -0.1, 0 2\" --mb=\"0.1, -0.1, 0 2\" --msize=4");
    }

    std::stringstream mass(vm["ma"].as<std::string>());
    std::vector<double> inA;

    std::transform(std::istream_iterator<std::string>(mass), {}, std::back_inserter(inA),
        [](const std::string& s) {
            return std::stod(s);
        });

    std::cout << "read matrix b: " << inA.size() << " elements" << std::endl;

    std::stringstream mbss(vm["mb"].as<std::string>());
    std::vector<double> inB;

    std::transform(std::istream_iterator<std::string>(mbss), {}, std::back_inserter(inB),
        [](const std::string& s) {
            return std::stod(s);
        });

    std::cout << "read matrix b: " << inB.size() << " elements" << std::endl;

    auto sz = vm["msize"].as<int>();

    auto params = polymatrix::mulDefaultParams();
    params.params.SetMultiplicativeDepth(2);
    params.params.SetBatchSize(4096);
    params.params.SetScalingModSize(50);
    params.params.SetFirstModSize(55);
    params.params.SetScalingTechnique(lbcrypto::ScalingTechnique::FLEXIBLEAUTO);
    params.params.SetRingDim(65536);
    params.rotationKeys = std::vector<int32_t>{1, 64 ,-1 ,-2 ,-4 ,-8 ,-16 ,-32 ,-64 ,-128, -256, -512, -1024, -2048};

    auto context = polycircuit::createCKKSContext(params);
    auto keys = context.keys;

    auto mul = polymatrix::Mul(context.cc, keys.publicKey);

    auto c1 = context.encrypt(inA);
    auto c2 = context.encrypt(inB);

    std::cout << "encrypt data " << std::endl;

    polycircuit::CKKSArguments args(std::vector<polycircuit::CKKSCiphertext>{c1, c2}, std::vector<double>{static_cast<double>(sz)});
    auto result = mul.evaluate(args);

    std::cout << "solved" << std::endl ;

    auto decrypted = context.decrypt(result, keys.secretKey, sz);
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

