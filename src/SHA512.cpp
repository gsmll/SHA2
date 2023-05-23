#include "hash/SHA512.hpp"

#include <cstring>
#define DEBUG true

#ifdef DEBUG
#include <iomanip>
#include <bitset>
#endif

namespace _details 
{

    [[nodiscard]] std::uint8_t* preprocess_sha512(const char* input, std::size_t* blk_total)
    {
        constexpr std::size_t BLOCK_BYTE_SIZE = 1024 / 8;

        std::size_t strsize = std::strlen(input);
        *blk_total = (strsize + 8 + 1) / 128 + ( (strsize + 8 + 1) % 128 ? 1 : 0 );
        std::uint8_t* blks = new std::uint8_t[BLOCK_BYTE_SIZE * *blk_total]{ }; // paren important        
        std::memcpy(blks, input, strsize);
        blks[strsize] |= 1 << 7;
        std::uint64_t strbitsize = to_big_endian(8ul * strsize);
        std::memcpy(blks + *blk_total * BLOCK_BYTE_SIZE - sizeof(std::uint64_t), &strbitsize, sizeof(std::uint64_t));

#ifdef DEBUG
        std::cout << "PADDED MESSAGE\n" << std::hex << std::setfill('0');
        for (std::size_t i = 0; i < BLOCK_BYTE_SIZE * *blk_total; ++i)
        {
            std::cout << std::setw(2) << +blks[i];
            if (i % 4 == 3) std::cout << "\n";
        }
        std::cout << "\n" << std::dec << std::setfill(' ');
#endif

        return blks;
    }

   
}