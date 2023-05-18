#ifndef SHA256_HPP
#define SHA256_HPP

#include "Utility.hpp"
#include "Hash.hpp"

namespace _details
{
    std::uint8_t* preprocess_sha256(const char* input, std::size_t blks )

    Hash<256> general_sha256(const char* input);

#ifdef __AVX2__
    Hash<256> simd_sha256(const char* input);
#endif

#ifdef __SHA__
    Hash<256> instruction_sha256(const char* input);
#endif

}

Hash<256> sha256(const char* input);

#endif