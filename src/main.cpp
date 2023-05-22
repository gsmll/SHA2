#include <iostream>

#include "Hash.hpp"
#include "hash/SHA256.hpp"
#include "hash/SHA224.hpp"

#define TEST(expr, msg)          \
if (expr) std::cout << "PASSED TEST [" << msg << "]\n"; \
else std::cout << "FAILED TEST [" << msg << "]\n"; 

int main()
{
    std::size_t blk_total;
    std::uint8_t* res = _details::preprocess_sha512("abc", &blk_total);

    using byte_to_binary = std::bitset<8>;
    for (std::size_t i = 0; i < blk_total * 1024 / 8; ++i)
    {
        std::cout << byte_to_binary{ res[i] } << " ";
    }
    TEST(sha256(buffer) == 0xcdc76e5c9914fb9281a1c7e284d73e67f1809a48a497200e046d39ccc7112cd0_hash, "SHA256 LONG LONG MESSAGE TEST");
    delete[] buffer;

    TEST(sha224("hello world") == 0x2f05477fc24bb4faefd86517156dafdecec45b8ad3cf2522a563582b_hash, "SHA224 SHORT MESSAGE TEST");
    TEST(sha224("abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq") == 0x75388b16512776cc5dba5da1fd890150b0c6455cb4f58b1952522525_hash, "SHA224 MEDIUM MESSAGE TEST");
}