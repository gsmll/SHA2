# Work Log

## Jeffrey Jiang

### 5/17/2023

Began preliminary work with populating the [makefile](./makefile), defining [Utility.hpp](include/Utility.hpp), and implementing the `Hash<N>` object representing an N-bit hash in [Hash.hpp](include/Hash.hpp). 

### 5/18/2023

Completed implementing `preprocess_sha256` which is used in the SHA256 algorithm.

### 5/19/2023

Completed implementing first version of SHA256 in [SHA256.cpp](src/SHA256.cpp).

### 5/20/2023

Implemented SHA224 in [SHA224.cpp](src/SHA224.cpp). Optimized SHA256 and SHA224 by ~10%. Implemented code for profiling code in [ProfileUtil.hpp](include/ProfileUtil.hpp) and an user-defined literal for hashes in [Hash.hpp](include/Hash.hpp) that is useful for testing code.

### 5/22/2023

Attempted SIMD implementation of SHA256 (Failed miserably). Profiled program.

### 5/23/2023

Continued working on SIMD implementation of SHA256 in [SHA256.cpp](src/SHA256.cpp). Also began working on SHA256 implementation using machine instructions in [SHA256.cpp](src/SHA256.cpp).

## GROUP MEMBER 2

### date x

info

### date y

info
