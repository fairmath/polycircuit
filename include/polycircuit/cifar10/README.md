# CIFAR-10 Image Classification component

*This is a stub repository for the FHERMA CIFAR-10 Image Classification
challenge https://fherma.io/challenges/652bf663485c878710fd0209 which is live March-Jun 2024.*

---

## Overview

[CIFAR-10](https://www.cs.toronto.edu/~kriz/cifar.html) is a widely recognized dataset comprising 60,000 color images of
size 32x32 pixels, categorized into 10 classes (such as automobiles, airplanes, dogs, etc.).
This dataset serves as a standard benchmark for machine learning algorithms in computer vision.

The goal of the challenge is to develop and implement a machine-learning model capable of efficiently classifying
encrypted CIFAR-10 images without decrypting them.

## Challenge Info:

1. **Challenge type**: This challenge is a White Box.
   The project with source code is required.
2. **Encryption Scheme**: CKKS.
3. **Supported libraries
   **: [OpenFHE](https://github.com/openfheorg/openfhe-development), [Lattigo](https://github.com/tuneinsight/lattigo).
4. **Input**:
    - Encrypted image
    - Cryptocontext
    - Public key
    - Multiplication key
    - Galois keys
5. **Output**: Encrypted classification result.

## How to Participate

This is a **White Box** challenge.
We've prepared a guide on how to participate in these types of challenges, please see
our [User Guide](https://fherma.io/how_it_works).

## Encoding Technique

We utilize the following class indexing for the CIFAR-10 dataset.

| Index | Class      | 
|-------|------------| 
| 0     | Airplane   |
| 1     | Automobile |
| 2     | Bird       |
| 3     | Cat        |
| 4     | Deer	      |
| 5     | Dog        |
| 6     | Frog       |
| 7     | Horse      |
| 8     | Ship       |
| 9     | Truck      |

### Input

Each image is encoded as a real vector with a dimension of 3072=3x1024.
The initial 1024 slots denote the red channel, the subsequent ones denote green, and the final segment denotes blue.
Each slot stores value in the range of [0, 255].

If you need the data to be packaged in a different way, please open an issue on
the [GitHub](https://github.com/Fherma-challenges/cifar-10).

### Output

The outcome of the computation is governed by the initial 10 slots in the resultant ciphertext.
If the input image belongs to class "i", then within the first 10 slots of the resultant vector, the maximum value must
be located in slot "i."

## Parameters

Parameters used to generate cryptocontext, keys and ciphertext can be changed through the `config.json` file located in
the project root.
Parameters must provide security of at least 128 bits.

## Implementation

Your implementation could be here!

## Test Environment

The solution will be tested inside a docker container.
The following libraries/packages will be used in the testing environment:

- **OpenFHE**: v1.1.2
- **OpenFHE-Python**: v0.8.4
- **Lattigo**: v5.0.2

### Command-Line Interface for Application Testing

#### OpenFHE

The application should support the following command-line interface (CLI) options:

- **--input** [path]: Specifies the path to the file containing the encrypted test image.
- **--output** [path]: Specifies the path to the file where the result should be written.
- **--cc** [path]: Indicates the path to the Cryptocontext file serialized in **BINARY** form.
- **--key_public** [path]: Specifies the path to the Public Key file.
- **--key_mult** [path]: Specifies the path to the Evaluation (Multiplication) Key file.
- **--key_rot** [path]: Specifies the path to the Rotation Key file.

#### Lattigo

The application should support the following command-line interface (CLI) options:

- **--input** [path]: Specifies the path to the file containing the encrypted test image.
- **--output** [path]: Specifies the path to the file where the result should be written.
- **--cc** [path]: Indicates the path to the Cryptocontext file serialized in **BINARY** form.
- **--key_eval** [path]: defines the path to the file where `MemEvaluationKeySet` object is
  serialized. `MemEvaluationKeySet` contains the evaluation key and Galois keys.

## Example

If the input image is classified as airplane (class 0), then the following outcomes are considered correct:

| 0.78 | 0.23 | 0.56 | 0.75 | 0 | 0.1 | 0.23 | 0.56 | 0.43 | 0.3 | ... |
|------|------|------|------|---|-----|------|------|------|-----|-----|

| 0.98 | 0.23 | 0.26 | 0.93 | 0 | 0.1 | 0.23 | 0.56 | 0.43 | 0.3 | ... |
|------|------|------|------|---|-----|------|------|------|-----|-----|

| 0.48 | 0.23 | 0.16 | 0.17 | 0 | 0.1 | 0.23 | 0.26 | 0.43 | 0.3 | ... |
|------|------|------|------|---|-----|------|------|------|-----|-----|

## Useful Links

* [OpenFHE](https://github.com/openfheorg/openfhe-development)
* [OpenFHE Python](https://github.com/openfheorg/openfhe-python)
* [Lattigo](https://github.com/tuneinsight/lattigo)

## Help

If you have any questions, you can:

* Contact us by email: support@fherma.io
* Ask a question in our [Discord](https://discord.gg/NfhXwyr9M5).
* Open an issue in the [GitHub Repository](https://github.com/Fherma-challenges/cifar-10).
* Use [OpenFHE Discourse](https://openfhe.discourse.group).
