# Matrix Multiplication component

The winning solution for the FHErma [Matrix Multiplication challenge](https://fherma.io/challenges/652bf669485c878710fd020b).

For a comprehensive analysis check out the [blog post](https://fherma.io/content/65de4152bfa5f4ea4471701e) by the challenge's winner, Aikata, a Ph.D. student at TU Graz.

## Overview

This component provides an efficient and scalable solution for encrypted matrix multiplication, with possible applications in encrypted machine learning and inference via fully homomorphic encryption schemes such as CKKS (a.k.a. Approximate Homomorphic Encryption). The proposed algorithm is quite simple, yet it achieves non-trivial results, especially when applied to higher packing scenarios; its effectiveness lies in its ability to provide favorable outcomes under these conditions.

## Performance and parallelization

This solution outperforms existing methods like [1](https://eprint.iacr.org/2023/1649.pdf) and [2](https://eprint.iacr.org/2018/1041.pdf) by consuming only $\mathcal{O}(\log_2{d})$ rotations while still requiring a multiplicative depth of two.

The optimized algorithm exhibits high parallelizability. To leverage it, consider using 'pragma omp parallel' before the `for` loops to enhance performance.

## Implementation

The solution is based on an adapted version of the Row-wise encoding algorithm from [this work](https://eprint.iacr.org/2023/1649.pdf), with optimization of the initial packing of duplicates of the same ciphertext (one original and one rotated).

**Row-Wise Encoding**
   - Depth of two multiplications using a simple row-wise encoding.
   - For a square matrix with dimensions $d\times d$, requires $2 d+3\log_2(d)-2$ rotations and $2d$ multiplications
   - Drawback: necessity for $d^3$ slots packing availability, which limits scalability.
   
The adapted technique is outlined in Algorithm~\ref{algo:mult_1}; it utilizes column and row masks ($\pi_i$ and $\psi_i$, respectively). The complexity of this adaptation is $2d+d\log_2{d}-1$ rotations and $3d$ multiplications.

**Algorithm 1** Matrix.Mult (1 Matrix packing version of [this work](https://eprint.iacr.org/2023/1649.pdf))\
**Require:** $A,B \leftarrow$ row_enc $(\mathtt{A_{d\times d}},\mathtt{B_{d\times d}})$\
**Out:** $C=$ row_enc $(\mathtt{A_{d\times d}}\times\mathtt{B_{d\times d}})$

`// Preprocess A`\
1: **for** $j=0$ to $d-1$ **do**\
2: &nbsp;&nbsp;&nbsp;&nbsp;$\tilde{A}[j] \leftarrow  \texttt{cMult}(A, \pi_{j} )$\
3: &nbsp;&nbsp;&nbsp;&nbsp;$\tilde{A}[j] \leftarrow  \texttt{Rot}(\tilde{A}[j],-j)$\
4: &nbsp;&nbsp;&nbsp;&nbsp;**for** $i=0$ to $\log_2(d)-1$}	**do**\
5: &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;$\tilde{A}[j] +=  \texttt{Rot}(\tilde{A}[j], -(2^i) )$

`// Preprocess B`\
6: **for** $j=0$ to $d-1$ **do**\
7: &nbsp;&nbsp;&nbsp;&nbsp;$\tilde{B}[j] \leftarrow  \texttt{cMult}(B, \psi_{j} )$\
8: &nbsp;&nbsp;&nbsp;&nbsp;$\tilde{B}[j] \leftarrow  \texttt{Rot}(\tilde{B}[j],-j*d)$\
9: &nbsp;&nbsp;&nbsp;&nbsp;**for** $i=0$ to $\log_2(d)-1$ **do**\
10: &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;$\tilde{B}[j] += \texttt{Rot}(\tilde{B}[j], -(2^i)*d )$
 
`// Compute C`\
11: **for** $j=0$ to $d-1$ **do**\
12: &nbsp;&nbsp;&nbsp;&nbsp;$C += \texttt{cMult}(\tilde{A}[j],\tilde{B}[j] )$

Since it is possible to pack two matrices into one ciphertext, the algorithm was optimized to consume $2d+d\log_2{d}-1$ rotations and $\frac{5d}{2}$ multiplications. This is achieved by aligning rotations at Steps 3 and 8 and then packing two ciphertexts, thus leading to a significant reduction in the number of rotations. The Algorithm~\ref{algo:mult_2} presents this approach, where both ciphertexts $A$ and $B$ undergo pre-processing. Following this packing strategy, only $d\log_2{d}+1$ rotations and $\frac{3d}{2}$ multiplications are required for the subsequent steps.

**Algorithm 2** Optimized.Matrix.Mult\
**Require:** $A,B \leftarrow$ row_enc $(\mathtt{A_{d\times d}},\mathtt{B_{d\times d}})$\
**Out:** $C=$ row_enc $(\mathtt{A_{d\times d}}\times\mathtt{B_{d\times d}})$

`// Preprocess A`\
1: $A +=  \texttt{Rot}(A, -d*d+1 )$\
2: **for** $j=0$ to $(d/2)-1$} **do**\
3: &nbsp;&nbsp;&nbsp;&nbsp;$\tilde{A}[j] \leftarrow  \texttt{cMult}(A, \pi_{j,j+d*d} )$\
4: &nbsp;&nbsp;&nbsp;&nbsp;$\tilde{A}[j] \leftarrow  \texttt{Rot}(\tilde{A}[j],-2j)$\
5: &nbsp;&nbsp;&nbsp;&nbsp;**for** $i=0$ to $\log_2(d)-1$ **do**\
6: &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;$\tilde{A}[j] +=  \texttt{Rot}(\tilde{A}[j], -(2^i) )$

`// Preprocess B`\
7: $B +=  \texttt{Rot}(B, -d*d+d )$\
8: **for** $j=0$ to $(d/2)-1$ **do**\
9: &nbsp;&nbsp;&nbsp;&nbsp;$\tilde{B}[j] \leftarrow  \texttt{cMult}(B, \psi_{j,j+d*d} )$

10: &nbsp;&nbsp;&nbsp;&nbsp;$\tilde{B}[j] \leftarrow  \texttt{Rot}(\tilde{B}[j],-2j*d)$\
11: &nbsp;&nbsp;&nbsp;&nbsp;**for** $i=0$ to $\log_2(d)-1$ **do**\
12: &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;$\tilde{B}[j] +=  \texttt{Rot}(\tilde{B}[j], -(2^i)*d )$

`// Compute C`\
13: **for** $j=0$ to $(d/2)-1$ **do**\
14: &nbsp;&nbsp;&nbsp;&nbsp;$C += \texttt{cMult}(\tilde{A}[j],\tilde{B}[j] )$\
15: $C += \texttt{Rot}(c,d* d )$

## Future improvements

The proposed approach, tailored to the constraints of the FHErma challenge, presents further opportunities for exploration. Extending the algorithm to handle rectangular matrices and diverse scenarios involving smaller matrix filters applied to a matrix could enhance its versatility and applicability in encrypted matrix multiplication contexts.