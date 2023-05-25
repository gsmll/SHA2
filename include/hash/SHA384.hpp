#ifndef SHA384_HPP
#define SHA384_HPP

#include "Utility.hpp"
#include "Hash.hpp"

namespace _details
{
    [[nodiscard]] std::uint8_t* preprocess_sha384(const char* input, std::size_t* blk_total);

    [[nodiscard]] Hash<384> general_sha384(const char* input);

#ifdef __AVX2__
    [[nodiscard]] Hash<384> simd_sha384(const char* input);
#endif

#ifdef __SHA__
    [[nodiscard]] Hash<384> instruction_sha384(const char* input);
#endif

}

[[nodiscard]] Hash<384> sha384(const char* input);

#endif