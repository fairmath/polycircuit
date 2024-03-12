# Logistic Function aka sigmoid component

The winning solution for the FHERMA [Logistic Function challenge](https://fherma.io/challenges/652bf648485c878710fd0208).

For a more comprehensive analysis check out the [blog post](https://fherma.io/content/65de3d6abfa5f4ea4471701a) by the challenge's winner, [Aikata](https://www.iaik.tugraz.at/person/aikata-aikata/), a Ph.D. student at TU Graz.

## Overview

This component provides a solution for approximating logistic aka sigmoid function, which is the basis for logistic regression-based machine learning, also commonly employed as a non-linear activation function in neural networks.
The proposed algorithm provides an accuracy of $99.9988\%$ with a multiplicative depth of 7 and an accuracy of $96.6\%$ with a multiplicative depth of 4.

## Implementation

|![sigmoid_2](https://hackmd.io/_uploads/BkdAYe83a.png)|![sigmoid_1](https://hackmd.io/_uploads/HkT6FgLhp.png)|
:-------------------------:|:-------------------------:
|*a) Sigmoid and its function approximation using Chebyshev series (64,128).*| *b) Sigmoid and its function approximation using Chebyshev series (8,16).*|

**Figure 1:** Sigmoid function and its approximation using Chebyshev series.

One common method for approximating the logistic function involves employing the Chebyshev series.
However, the Chebyshev series provides results over the domain [−1, 1].
To apply the Chebyshev series to broader ranges (e.g., $[a,b]$), the input polynomial (e.g., $x$) is scaled to bring it to the interval $[-1,1]$ as outlined below, ensuring the applicability of the Chebyshev approximation.

$$
x' = \frac{2x - (b+a)}{b-a}
$$

$$
x' = \frac{x}{\omega}
$$

$$
\text{ when } |a|=|b|, a \neq b,\text{ and }\omega=|b|
$$

The scaled ciphertext $x'$ is subsequently employed for the approximation.
Note that this scaling results in the loss of one multiplicative depth.
This aspect is explicitly mentioned in the [library documentation](https://github.com/openfheorg/openfhe-development/blob/main/src/pke/examples/FUNCTION_EVALUATION.md) for function evaluation.

Consequently, when presented with a challenge specifying a multiplicative depth $d$ over an arbitrary range $[a,b]$, such that $a\neq-1$ and $b\neq1$, the available Chebyshev series approximation technique allows us to evaluate up to a depth of $d-1$ at best.
To fully exploit this using OpenFHE, the technique described in the [SIGN challenge solution](https://fherma.io/content/65de3f45bfa5f4ea4471701c) can be applied.
However, it becomes evident that although reducing the depth from 7 to 6 $(2^7=128 \rightarrow 2^6=64)$, as shown in **Figure 1(a)** does not result in a significant loss of accuracy, dropping from 4 to 3 ($2^4 = 16 \rightarrow 2^3 = 8$) entails a considerable loss (**Figure 1(b)**).
Therefore, this algorithm focuses on enhancing the existing technique to yield the best approximation results.
