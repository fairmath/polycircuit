# FHE Components library

This repository is an initial phase of building an application layer FHE Components library for developers and serves as
the home to the award-winning solutions from the [FHERMA](https://fherma.io/) challenges platform.
The library will be constantly extended by adding new FHE components.

# Installation

To use library components, you'll need to install several dependencies and Polycircuit library itself.

## Prerequisites

Ensure you have the following dependencies installed:

* `CMake >= 3.5.1`
* `G++ >= 11.4`
* `Git >= 2.34.1`

## Library installation

To install the header-only Polycircuit library, follow these steps:

1. Clone the repository

```bash
git clone https://github.com/fairmath/polycircuit.git
cd polycircuit
```

2. Configure CMake and install Polycircuit library in /usr/local/include

```bash
cmake .
cmake --build . --target install
```

You can also set a different installation directory:

```bash
cmake . -DCMAKE_INSTALL_PREFIX:PATH=/path/to/include
cmake --build . --target install
```

# Build and run the examples

The examples demonstrate the basic use of a particular component.
To build and run examples, you'll need to install several dependencies.

## Prerequisites

Ensure you have the following dependencies installed:

* `CMake >= 3.5.1`
* `G++ >= 11.4`
* `Git >= 2.34.1`
* `Boost >= 1.74.0`
* `OpenFHE >= 1.1.4`

## Building and running the MatrixMultiplicationUsage example:

1. Build:

```bash
cd examples/MatrixMultiplicationUsage
cmake -B ./build && make -C ./build -j$(nproc)
```

2. Run (for more info about accepting parameters, see help):

```bash
./build/MatrixMultiplicationUsage --help
```

# Contributing to the Library

There are two ways to contribute to Polycircuit:

- Create PR with a new component or improvements for an existing one;
- Participate in the challenges on the [FHERMA platform](https://fherma.io/challenges).

# Stay Connected

Join our vibrant community to stay updated on the latest developments, participate in discussions, and connect with
fellow FHE enthusiasts:

* Discord: Join our [Discord server](https://discord.com/invite/NfhXwyr9M5) for real-time chat, Q&A, and community
  events.
* Twitter: Follow us on [X](https://twitter.com/FairMath) for the latest news, updates, and interesting tidbits from the
  world of Fully Homomorphic Encryption.
* LinkedIn: Join our [LinkedIn group]() to stay in touch with the team and the updates.
* Visit our [Fair Math blog](https://fairmath.xyz/blog) and [FHERMA challenges wrap-ups](https://fherma.io/content) for
  in-depth articles, feature announcements, tutorials, and insights into the future of privacy-preserving technologies.
