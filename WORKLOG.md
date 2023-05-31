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

### 5/24/2023

Continued working the SHA intrinsics version of SHA256. Did not make any real progress.

### 5/25/2023

Finally completed SHA intrinsics verson of SHA256 as found in [SHA256.cpp](src/SHA256.cpp). Also performed tests and profiled the code. 

### 5/26/2023

Began planning pseudocode for new SIMD algorithm implementation of SHA256 and SHA224.

### 5/27/2023

Implemented new faster SIMD algorithm for SHA256/224 in [SHA256.cpp](src/SHA256.cpp) and [SHA224.cpp](src/SHA224.cpp). Sees around 10% to 25% speed increase over the serial implementation.

### 5/28/2023

Implemented minor quality of life command line argument options in [ArgumentParser.cpp](src/shashaker/ArgumentParser.cpp). Started generalizing hash cracker in [Process.cpp](src/shashaker/Process.cpp).

### 5/29/2023

Implemented multithreaded hash cracker tool. Need to correct some issues such as data races with printing to console, but that is for another time.

### 5/30/2023

Started writing up presentation in [PRESENTATION.md](PRESENTATION.md)

### 5/31/2023

Finished presentation in [PRESENTATION.md](PRESENTATION.md)

### 6/1/2023

Revised and updated [README.md](README.md) and [HOMEWORK.md](HOMEWORK.md). Added `gpg` files for backup incase SSH server does not work.

## GROUP MEMBER 2

### date x

info

### date y

info
