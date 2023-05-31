#ifndef SHA224_HPP
#define SHA224_HPP

#include "Utility.hpp"
#include "Hash.hpp"

namespace _details
{
    [[nodiscard]] std::uint8_t* preprocess_sha224(const char* input, std::size_t* blk_total);

    [[nodiscard]] Hash<224> general_sha224(const char* input);

#ifdef __AVX2__
    [[nodiscard]] Hash<224> simd_sha224(const char* input);
#endif

#ifdef __SHA__
    [[nodiscard]] Hash<224> instruction_sha224(const char* input);
#endif

}

[[nodiscard]] Hash<224> sha224(const char* input);

#endif