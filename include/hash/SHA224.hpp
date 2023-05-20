#ifndef SHA224_HPP
#define SHA224_HPP

#include "Utility.hpp"
#include "Hash.hpp"
#include "SHA256.hpp"

namespace _details
{
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