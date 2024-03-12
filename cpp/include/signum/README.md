# Sign Evaluation component

The winning solution for the FHERMA [Sign Evaluation challenge](https://fherma.io/challenges/652bf668485c878710fd020a).

For a more comprehensive analysis check out the [blog post](https://fherma.io/content/65de3f45bfa5f4ea4471701c) by the challenge's winner, [Aikata](https://www.iaik.tugraz.at/person/aikata-aikata/), a Ph.D. student at TU Graz.

## Overview

This component provides a solution for approximating the sign function, also known as signum, which determines the sign of a value.
It has significant applications in machine learning, serving as a foundational element for non-linear activation functions like the Rectified Linear Unit (ReLu) or Max Pooling.

One prevalent approach for effectively approximating the sign function involves the Chebyshev series, known for its universal applicability.
Notably, OpenFHE has already incorporated an implementation of the Chebyshev series that evaluates the Homomorphic modular reduction during the Bootstrapping procedure.
The accuracy reached with the proposed algorithm goes up to $99.97\%$, as shown in **Figure 1**.

## Implementation

![cheb_approx](https://hackmd.io/_uploads/Bkj5FNI3T.png)

**Figure 1:** Function approximations for the sigmoid function using Chebyshev series (8,16).

Employing a straightforward approximation of the sign function through the Chebyshev series evaluation within the OpenFHE library yields an accuracy of $99.96\%$.
The Chebyshev series can only be evaluated up to the coefficient $1006$ when approximating $\text{Tanh}(x\times\texttt{RAND_MAX})$ using OpenFHE.
The resulting approximation is illustrated in **Figure 1**.
To enhance the computational capabilities of OpenFHE, we conducted an analysis to determine the feasibility of evaluating additional Chebyshev series.
Since the signum is an odd function, the even degree coefficients do not contribute to its approximation, so there is no need to evaluate the even terms of the Chebyshev series.

Building on this observation, we explored whether it is possible to evaluate $c \times\textbf{T}_{1009}$ while adhering to the multiplicative depth limitation of 10, where $c$ is the coefficient resulting from the function approximation.
We utilize the properties of the Chebyshev series and write down recursive relations as follows:

\begin{align}
    \text{c}\times T_{1009} && \rightarrow && \text{2c}\times T_{512}\times T_{497}-\text{c}\times T_{15} && \rightarrow &&   T_{512}\times(\text{2c}\times T_{497})-\text{c}\times T_{15}
\end{align}

\begin{align}
    \text{2c}\times T_{497} && \rightarrow &&  \text{4c}\times T_{256}\times T_{241}-\text{2c}\times T_{15} && \rightarrow  &&  T_{256}\times(\text{4c}\times T_{241})-\text{2c}\times T_{15}
\end{align}

\begin{align}
    \text{4c}\times T_{241} && \rightarrow  && \text{8c}\times T_{128}\times T_{113}-\text{4c}\times T_{15} && \rightarrow &&   T_{128}\times(\text{8c}\times T_{113})-\text{4c}\times T_{15}
\end{align}

\begin{align}
    \text{8c}\times T_{113} && \rightarrow &&  \text{16c}\times T_{64}\times T_{49}-\text{8c}\times T_{15} && \rightarrow &&   T_{64}\times(\text{16c}\times T_{49})-\text{8c}\times T_{15}
\end{align}

\begin{align}
    \text{16c}\times T_{49} && \rightarrow &&  \text{32c}\times T_{32}\times T_{17}-\text{16c}\times T_{15} && \rightarrow &&   T_{32}\times(\text{32c}\times T_{17})-\text{16c}\times T_{15}
\end{align}

\begin{align}
    \text{32c}\times T_{17} && \rightarrow &&  \text{64c}\times T_{16}\times T_{1}-\text{32c}\times T_{15} && \rightarrow &&   T_{16}\times(\text{64c}\times T_{1})-\text{32c}\times T_{15}
\end{align}

Upon the initial breakdown of $\textbf T_{1009}$, we observed that the series terms to be multiplied ($\textbf T_{512}$, $\textbf T_{497}$) were at the same depth of 9.
Unfortunately, this configuration made it impossible to perform a third multiplication with the coefficient $\text{c}$.
Therefore, we recursively broke down the expression until we achieved terms ($\textbf{T}_{16}$, $\textbf{T}_{1}$) at different depths (4,1).
At this stage, we can multiply $\text{c}$ with $\textbf T_{1}$, bringing it to a depth of 1.
Subsequently, we can compute the recursive breakdown upwards to obtain $c \times \textbf T_{1009}$.

The same methodology is applied to evaluate all the remaining Chebyshev series coefficients - $\textbf T_{1011}$, $\textbf T_{1013}$, $\textbf T_{1015}$, $\textbf T_{1017}$, $\textbf T_{1019}$, $\textbf T_{1021}$, and $\textbf T_{1023}$.
The recursion must be further broken down to evaluate higher degree coefficients.
This additional evaluation increases accuracy from $99.96\%$ to $99.97\%$.

## Future improvements

The newfound capability to evaluate the full depth presents an intriguing opportunity to explore potential improvements or attempt to reproduce the results outlined in [this paper](https://eprint.iacr.org/2020/834).
This increased depth computation also serves to improve the efficiency of other function approximations, for example, the logistic function.
