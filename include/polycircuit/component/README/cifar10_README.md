# CIFAR-10 classification

☀️ *This component was developed during the FHERMA CIFAR-10 image classification challenge in Mar - Jun 2024.*

This README describes the winning solution for the [FHERMA](https://fherma.io) [CIFAR-10 classification challenge](https://fherma.io/challenges/652bf663485c878710fd0209).

For a more comprehensive analysis of the solution, check out the blog posts by the challenge winners: **hita** (Hieu Nguyen from the University of Technology Sydney) and [by the team of **Valentina Kononova**, **osmenojka** (Dmitry Tronin) and **Dmitrii Lekomtsev**](https://fherma.io/content/66d86ed4e4477f9e186fa08f).

## Image classification on encrypted dataset via CKKS scheme

[CIFAR-10](https://www.cs.toronto.edu/~kriz/cifar.html) is a widely recognized dataset comprising 60,000 color images of size 32x32 pixels, categorized into 10 classes such as automobiles, airplanes, dogs, etc.
This dataset serves as a standard benchmark for machine learning algorithms in computer vision.

The goal of the challenge was to develop and implement a machine-learning model capable of efficiently classifying CIFAR-10 images encrypted with the **CKKS homomorphic encryption scheme** without decrypting them, showcasing the intersection of cryptography and machine learning, particularly in privacy-preserving computations.

## Challenge requirements
### Input and encoding technique

The following class indexing was utilized for the CIFAR-10 dataset:

| Index | Class| 
| -------- | -------- | 
| 0     | Airplane    |
| 1     | Automobile  |
| 2     | Bird   |
| 3     | Cat    |
| 4     | Deer	    |
| 5     | Dog    |
| 6     | Frog    |
| 7     | Horse    |
| 8     | Ship    |
| 9     | Truck    |

Each image for the input is encoded as a real vector with a dimension of 3072=3x1024.
The initial 1024 slots denote the red channel, the subsequent ones denote green, and the final segment denotes blue.
Each slot stores a value in the range of [0, 255].

### Output

The outcome of the computation is governed by the initial 10 slots in the resultant ciphertext.
If the input image belongs to class `i`, then within the first 10 slots of the resultant vector, the maximum value must be located in slot `i`.

### Example

For a classified image of an airplane (class 0), correct outputs might look like:

| 0.78 | 0.23 | 0.56 | 0.75 | 0 | 0.1 | 0.23 | 0.56 | 0.43 | 0.3 | ... |
|---|---|---|---|---|---|---|---|---|---|---|

| 0.98 | 0.23 | 0.26 | 0.93 | 0 | 0.1 | 0.23 | 0.56 | 0.43 | 0.3 | ... |
|---|---|---|---|---|---|---|---|---|---|---|

| 0.48 | 0.23 | 0.16 | 0.17 | 0 | 0.1 | 0.23 | 0.26 | 0.43 | 0.3 | ... |
|---|---|---|---|---|---|---|---|---|---|---|

## Machine Learning Model

A neural network with one hidden layer was employed for image classification. The chosen model architecture was intentionally simple to align with the encrypted nature of the data and the constraints of homomorphic encryption. 

## Operations on Encrypted Data

Given the homomorphically encrypted nature of the CIFAR-10 images, several custom operations were necessary to handle the data without decryption:

- **EvalSum**: Implemented manually as sum keys were not provided.
- **DotProduct**: Custom implementation of EvalInnerProduct was necessary for encrypted operations.

These operations were critical to ensuring that the model could process the encrypted data correctly and efficiently.

## Optimizations

While neural networks typically benefit from parallelism, initial attempts to accelerate computations were hindered by Python’s pickle serialization limitations. To overcome this:

- **Parameter Adjustments**: We optimized `log_q` and `log_n` parameters and further reduced the number of multiplications.
- **Performance Testing**: Comprehensive unit, integration, and performance tests were implemented to track improvements and ensure accuracy.

These optimizations led to an additional 30% improvement in performance.

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

- **--input** [path]: path to the encrypted test image file.
- **--output** [path]: path to the output file for results.
- **--cc** [path]: path to the **cryptocontext** file in **BINARY** form.
- **--key_public** [path]: path to the **public key** file.
- **--key_mult** [path]: path to the **evaluation (multiplication) ke**y file.
- **--key_rot** [path]: path to the **rotation key** file.

### Parameters

Parameters that are used to generate crypto context, keys, and ciphertext are specified in the config file **config.json** located in the project root.
See the **Parameters** section in our [FHERMA participant guide](https://fherma.io/how_it_works) for more info on what can be configured.

## Useful Links

* [FHERMA participation guide](https://fherma.io/how_it_works)—more on FHERMA challenges
* [OpenFHE](https://github.com/openfheorg/openfhe-development) repository, README, and installation guide
* [OpenFHE Python](https://github.com/openfheorg/openfhe-python)
* [OpenFHE Rust](https://github.com/fairmath/openfhe-rs), its tutorial and documentation
* [OpenFHE AAAI 2024 Tutorial](https://openfheorg.github.io/aaai-2024-lab-materials/)—Fully Homomorphic Encryption for Privacy-Preserving Machine Learning Using the OpenFHE Library
* [A vast collection of resources](https://fhe.org/resources) collected by [FHE.org](http://FHE.org), including tutorials and walk-throughs, use-cases and demos.
