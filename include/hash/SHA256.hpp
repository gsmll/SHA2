#ifndef SHA256_HPP
#define SHA256_HPP

#include "Utility.hpp"
#include "Hash.hpp"

namespace _details
{
    [[nodiscard]] std::uint8_t* preprocess_sha256(const char* input, std::size_t* blk_total);

    [[nodiscard]] Hash<256> general_sha256(const char* input);

#ifdef __AVX2__
    [[nodiscard]] Hash<256> simd_sha256(const char* input);
#endif

#ifdef __SHA__
    [[nodiscard]] Hash<256> instruction_sha256(const char* input);
#endif

}

[[nodiscard]] Hash<256> sha256(const char* input);

#endif