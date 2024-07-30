
# ReLU Component

☀️ *This component was developed during the FHERMA ReLU Challenge competition in Mar - Jun 2024.*

---

The winning solution for the FHERMA [ReLU Challenge](https://fherma.io/challenges/6542c282100761da3b545c3e).

For a more comprehensive analysis, check out the [blog post](LINK!) by the challenge winners, [Janis Adamek](https://rcs.mb.tu-dortmund.de/about-us/team/researchers/janis-adamek/), [Dieter Teichrib](https://rcs.mb.tu-dortmund.de/about-us/team/researchers/dieter-teichrib/), [Philipp Binfet](https://rcs.mb.tu-dortmund.de/about-us/team/researchers/philipp-binfet/), and [Moritz Schulze Darup](https://rcs.mb.tu-dortmund.de/about-us/team/heads/moritz-schulze-darup/) from Control and Cyberphysical Systems Group, TU Dortmund, Germany.

## Overview

This component provides a solution for approximating the Rectified Linear Unit (ReLU) function, a critical non-linear activation function used in neural networks, particularly within deep learning models. The ReLU function introduces non-linearity, allowing neural networks to learn complex patterns and representations from the data.

The ReLU function is mathematically defined as:

$$
\text{ReLU}(x) = \max(0, x)
$$

The challenge required the development of an algorithm capable of computing the ReLU function on data encrypted using the CKKS scheme. The encrypted implementation leverages polynomial approximations due to the fixed multiplicative depth constraint.

## Example

For a vector `[-0.45, 0, 0.23]`, applying the **ReLU** function should result in `[0, 0, 0.23]`.

## Implementation

For a multiplicative depth of 4, achieving a high accuracy is quite challenging. A standard Chebyshev approximation of order $n=2^d-1=15$ yielded an accuracy of approximately 63%. However, by formulating the problem as a mixed-integer linear program (MILP) and utilizing integer leading coefficients, the accuracy was improved to 88.4%.

The final polynomial approximation, illustrated in Figure 1, is:

$$
\begin{align*}
p(x)&=0.0324+0.5x+2.1348x^2-13.9209x^4+70.0983x^6-213.0967x^8+385.9436x^{10}\\
&-407.0473x^{12}+230.3549x^{14}-54x^{16}
\end{align*}
$$

|![Figure 1](https://github.com/user-attachments/assets/37f20b77-4a08-4b6a-a023-18ea405221ba)|
|:-------------------------:|
|*Figure 1: (left) Approximation of the ReLU function with a polynomial of order 16. (right) Polynomial approximation with error threshold.*|

## Test Environment 

The following versions of libraries/packages were used for running tests:
 - **OpenFHE**: v1.1.2

## Useful Links

* [OpenFHE](https://github.com/openfheorg/openfhe-development)
* [OpenFHE Python](https://github.com/openfheorg/openfhe-python)
* [Lattigo](https://github.com/tuneinsight/lattigo)

## References

[1] Eunsang Lee, Joon-Woo Lee, Jong-Seon No, and Young-Sik Kim. Minimax approximation of sign function by composite polynomial for homomorphic comparison. IEEE Transactions on Dependable and Secure Computing, 19(6):3711–3727, 2022.

[2] MOSEK ApS. The MOSEK optimization toolbox for MATLAB manual. Version 10.0, 2024. [https://docs.mosek.com/10.0/toolbox/index.html](https://docs.mosek.com/10.0/toolbox/index.html).
