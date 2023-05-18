#include <bitset>

#include "Hash.hpp"
#include "hash/SHA256.hpp"

int main()
{
    std::size_t blk_total;
    std::uint8_t* res = _details::preprocess_sha256("abc", &blk_total);

    using byte_to_binary = std::bitset<8>;
    for (std::size_t i = 0; i < blk_total * 512 / 8; ++i)
    {
        std::cout << byte_to_binary{ res[i] } << " ";
    }
    std::cout << "\n";
}