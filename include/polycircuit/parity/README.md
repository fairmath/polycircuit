# Parity component

*This is a stub repository for the future Parity component from the IBM challenge on
FHERMA https://fherma.io/challenges/65ef8c4c5428d672bcc3977b which is live on March-Jun 2024.*

*Link to the template repository: https://github.com/Fherma-challenges/parity*

---

## Overview

This challenge was developed by [IBM Research](https://research.ibm.com).
The objective of the challenge is to design an algorithm that evaluates the function `parity(x)` under CKKS.
The function `parity(x)` gets an integer and returns its least significant bit (LSB). In other words,

$$parity(x) = x \mod 2,$$

where $x \in \mathbb{Z}.$

The `parity` function is closely related to the bit extraction problem, where given an integer $x$ the goal is to find
its bit representation $x =\sum 2^i b_i$ which is useful in many cases, e.g., comparisons.
Thus, an efficient implementation of `parity(x)` would lead to an efficient implementation of bit extraction.

## Challenge Info:

1. **Challenge type:** This challenge is a White Box challenge.
   Participants are required to submit the project with their source code.
2. **Encryption Scheme:** CKKS.
3. **FHE Library:** OpenFHE, Lattigo.
4. **Input Data:** Encrypted vector $x = (x_1, \ldots)$, where $x_i \in [0,255]$.
5. **Output Data:** The outcome should be an encrypted vector $parity(x)= \left(parity\left(x_1\right), \ldots\right)$.

## How to Participate

This is a **White Box** challenge.
We've prepared a guide on how to participate in these types of challenges, please see
our [User Guide](https://fherma.io/how_it_works).

## Parameters of the Key

1. **Bootstrapping:** The key will support bootstrapping.
2. **Number of slots:** $2^{16}$.
3. **Multiplication depth:** 29.
4. **Fractional part precision (ScaleModSize):** 59 bits.
5. **First Module Size:** 60 bits.

## Parameters of the input

1. **Packing:** Each slot will contain one value $x_i$.
2. **Input range:** For each element $x_i=n_i + e_i$, where $n_i\in [0,255]$ is an integer, and $|e_i| < 10^{-5}$ is a
   small noise.

## Requirements of the output

1. **Packing:** Each slot will contain one value $y_i = parity(x_i)$, the parity of the corresponding slot in the input.
2. **Ouput range:** For each element $y_i=b_i + E_i$, where $y_i\in [0,1]$ is an integer, and $|E_i| < 10^{-2}$ is a
   small noise.

## Implementation

Your implementation could be here!

## Test Environment

Submissions will be evaluated on a single-core CPU.
The following libraries/packages will be used for generating test case data and for testing solutions:

- **OpenFHE:** v1.1.4
- **OpenFHE-Python:** v0.8.6
- **Lattigo:** v5.0.2
- **HElayers:** v1.5.3.1 [Download](https://ibm.github.io/helayers/ 'Download HElayers')
- **pyhelayers:** v1.5.3.1 [Download](https://ibm.github.io/helayers/ 'Download pyhelayers')

## Example

The executable will be run as follows:

```
./app --cc cc.bin --key_public pub.bin --key_mult mult.bin --input in.bin --output out.bin
```

An example of the message encrypted in `in.bin`:

`Input = [203.0000005, 102.0000008, 3.0000002, 56.9999994, 77.9999993, ...]`

An example output for this input:

`Output = [0.995, 0.008, 1.002, 1.004, -0.003, ...]`

## Useful Links

* [OpenFHE](https://github.com/openfheorg/openfhe-development)
* [OpenFHE Python](https://github.com/openfheorg/openfhe-python)
* [Lattigo](https://github.com/tuneinsight/lattigo)

## Help

If you have any questions, you can:

* Contact us by email: support@fherma.io
* Ask a question in our [Discord](https://discord.gg/NfhXwyr9M5).
* Open an issue in the [GitHub Repository](https://github.com/Fherma-challenges/parity/issues).
* Use [OpenFHE Discourse](https://openfhe.discourse.group).
