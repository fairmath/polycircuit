#include <iostream>
#include <sstream>

#include "polycircuit/lut/lut.hpp"

#include <openfhe/pke/constants-defs.h>
#include <openfhe/pke/cryptocontext.h>
#include <openfhe/pke/openfhe.h>
#include <openfhe/pke/scheme/ckksrns/gen-cryptocontext-ckksrns-params.h>

/*
minimim required CKKS params
{
  "indexes_for_rotation_key": [-3, -2, -1, 10, 20, 40, 50, 100, 200, 400, 800,
1600], "mult_depth": 5, "ring_dimension": 8192, "scale_mod_size": 25,
  "first_mod_size": 30,
  "batch_size": 4096,
  "enable_bootstrapping": false,
  "levels_available_after_bootstrap": 0,
  "level_budget": [0,0]
}
*/
int main(int argc, char* argv[]) try { return EXIT_SUCCESS; }

catch (const std::exception& ex) {
    std::cerr << ex.what() << std::endl;
    return EXIT_FAILURE;
}
catch (...) {
    std::cerr << "An unknown exception was thrown." << std::endl;
    return EXIT_FAILURE;
}
