#include "hash/SHA256.hpp"

#include <cstring>

namespace _details 
{

    std::uint8_t* preprocess_sha256(const char* input, std::size_t* blk_total)
    {
        constexpr std::size_t BLOCK_BYTE_SIZE = 512 / 8;

        std::size_t strsize = std::strlen(input);
        *blk_total = (strsize + 8 + 1) / 64 + ( (strsize + 8 + 1) % 64 ? 1 : 0 );
        std::uint8_t* blks = new std::uint8_t[BLOCK_BYTE_SIZE * *blk_total]{ }; // paren important        
        std::memcpy(blks, input, strsize);
        blks[strsize] |= 1 << 7;
        std::uint64_t strbitsize = to_big_endian(8ul * strsize);
        std::memcpy(blks + *blk_total * BLOCK_BYTE_SIZE - sizeof(std::uint64_t), &strbitsize, sizeof(std::uint64_t));

        return blks;
    }

}