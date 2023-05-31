#ifndef SHA512_HPP
#define SHA512_HPP

#include "Utility.hpp"
#include "Hash.hpp"

namespace _details
{
    [[nodiscard]] std::uint8_t* preprocess_sha512(const char* input, std::size_t* blk_total);

    [[nodiscard]] Hash<512> general_sha512(const char* input);
}

[[nodiscard]] Hash<512> sha512(const char* input);

#endif