# ReLU component

*This is a stub repository for the future ReLU component from the FHERMA ReLU challenge https://fherma.io/challenges/6542c282100761da3b545c3e which is live on March-Jun 2024.*
*Link to the template repository: https://github.com/Fherma-challenges/relu*

---

## Overview

The Rectified Linear Unit (ReLU) function is a fundamental activation function used extensively in neural networks, particularly in deep learning models.
It is a simple yet powerful non-linear function that introduces non-linearity into the network, allowing it to learn complex patterns and representations in the data.

The ReLU function is defined mathematically as:

ReLU(x) = \max(0, x)

The objective of the challenge is to create an algorithm capable of computing the ReLU function on data encrypted with CKKS.

## Challenge Info:

1. **Challenge type:** This challenge is a Black Box.
Participants are only required to submit the resultant ciphertext.
Code or implementation details are not required during the challenge.
However, the project code will be asked from the winner of the competition.
2. **Encryption Scheme**: CKKS.
3. **Supported libraries**: [OpenFHE](https://github.com/openfheorg/openfhe-development), [Lattigo](https://github.com/tuneinsight/lattigo).
4. **Input Data**: 
    * Encrypted vector **X**
    * Cryptocontext
    * Public key
    * Multiplication (Relinearization) key
5. **Output Data**: The outcome should be an encrypted vector `ReLU(x)`.

*If additional rotation keys are needed, please open an issue on [GitHub](https://github.com/Fherma-challenges/relu) and we will provide them.*

## How to Participate

This is a **Black Box** challenge.
We've prepared a guide on how to participate in these types of challenges, please see our [User Guide](https://fherma.io/how_it_works).

## Encoding Technique

By default, we pack the input vector **X** in ciphertext as follows:
| x<sub>0</sub> | x<sub>1</sub> | x<sub>2</sub> | x<sub>3</sub> |...
|---|---|---|---|---

The resulting **ReLU(X)** vector should be packed in the same manner:

| ReLU(x<sub>0</sub>) | ReLU(x<sub>1</sub>) | ReLU(x<sub>2</sub>) | ReLU(x<sub>3</sub>) | ...
|---|---|---|---|---

*If you need the data to be packaged in a different format, please open an issue on [GitHub](https://github.com/Fherma-challenges/relu).*

## Implementation

Your implementation could be here!

## Test Environment 

The following libraries/packages were used for generating test cases: 
 - **OpenFHE**: v1.1.2
 - **Lattigo**: v5.0.2

Participants can use any third-party software and libraries to solve the challenge.
The only requirement is that the ciphertext uploaded to the platform should be compatible with the OpenFHE library v1.1 or Lattigo v5.0.2.

## Example

Suppose we have a vector `[-0.45, 0, 0.23]`.
After applying the **ReLU** function to each element, we get the vector `[0, 0, 0.23]`.

## Useful Links

* [OpenFHE](https://github.com/openfheorg/openfhe-development)
* [OpenFHE Python](https://github.com/openfheorg/openfhe-python)
* [Lattigo](https://github.com/tuneinsight/lattigo)

## Help

If you have any questions, you can:
 * Contact us by email: support@fherma.io
 * Ask a question in our [Discord](https://discord.gg/NfhXwyr9M5).
 * Open an issue in the [GitHub Repository](https://github.com/Fherma-challenges/relu).
 * Use [OpenFHE Discourse](https://openfhe.discourse.group).
