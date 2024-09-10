# Parity component

☀️ *This component was developed during the FHERMA Parity challenge by [IBM Research](https://research.ibm.com) in Mar - Jun 2024.*

---
This README describes the winning solution for the [FHERMA](https://fherma.io) [Parity challenge](https://fherma.io/challenges/652bf669485c878710fd020b).

## Overview

This challenge was developed by [IBM Research](https://research.ibm.com).
The objective of the challenge is to design an algorithm that evaluates the function `parity(x)` under CKKS.
The function `parity(x)` gets an integer and returns its least significant bit (LSB). In other words,

$$parity(x) = x \mod 2,$$

where $x \in \mathbb{Z}.$

The `parity` function is closely related to the bit extraction problem, where given an integer $x$ the goal is to find its bit representation $x =\sum 2^i b_i$ which is useful in many cases, e.g., comparisons.
Thus, an efficient implementation of `parity(x)` would lead to an efficient implementation of bit extraction.

## Challenge requirements
### Parameters of the input

1. **Packing:** Each slot will contain one value $x_i$.
2. **Input range:** For each element $x_i=n_i + e_i$, where $n_i\in [0,255]$ is an integer, and $|e_i| < 10^{-5}$ is a small noise.

### Requirements of the output

1. **Packing:** Each slot will contain one value $y_i = parity(x_i)$, the parity of the corresponding slot in the input.
2. **Ouput range:** For each element $y_i=b_i + E_i$, where $y_i\in [0,1]$ is an integer, and $|E_i| < 10^{-2}$ is a small noise.

### Example

The executable runs as follows:

```
./app --cc cc.bin --key_public pub.bin --key_mult mult.bin --input in.bin --output out.bin
```

An example of the message encrypted in `in.bin`:

`Input = [203.0000005, 102.0000008, 3.0000002, 56.9999994, 77.9999993, ...]`

An example output for this input:

`Output = [0.995, 0.008, 1.002, 1.004, -0.003, ...]`

## Running and testing
### Test environment

The solution was tested inside a Docker container using the following libraries/packages:

- **OpenFHE**: v1.1.4
- **OpenFHE-Python**: v0.8.6

### Hardware

**CPU**: 12 core
**RAM**: 54 GB

### Command-line interface

The component supports the following CLI options:

- **--input** [path]: path to the file containing the encrypted vector.
- **--output** [path]: path to the output file for results.
- **--cc** [path]: path to the **cryptocontext** file serialized in **BINARY** form.
- **--key_public** [path]: path to the **public key** file.
- **--key_mult** [path]: path to the **evaluation (multiplication) ke**y file.
- **--key_rot** [path]: path to the **rotation key** file.

### Parameters

Parameters that are used to generate crypto context, keys, and ciphertext are specified in the config file **config.json** located in the project root.
See the **Parameters** section in our [FHERMA participant guide](https://fherma.io/how_it_works) for more info on what can be configured.

## Useful links

* [FHERMA participation guide](https://fherma.io/how_it_works)—more on FHERMA challenges
* [OpenFHE](https://github.com/openfheorg/openfhe-development) repository, README, and installation guide
* [OpenFHE Python](https://github.com/openfheorg/openfhe-python)
* [OpenFHE Rust](https://github.com/fairmath/openfhe-rs), its tutorial and documentation
* [OpenFHE AAAI 2024 Tutorial](https://openfheorg.github.io/aaai-2024-lab-materials/)—Fully Homomorphic Encryption for Privacy-Preserving Machine Learning Using the OpenFHE Library
* [A vast collection of resources](https://fhe.org/resources) collected by [FHE.org](http://FHE.org), including tutorials and walk-throughs, use-cases and demos.
