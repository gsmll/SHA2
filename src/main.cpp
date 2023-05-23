#include <iostream>

#include "Hash.hpp"
#include "hash/SHA256.hpp"
#include "hash/SHA224.hpp"
#include "hash/SHA512.hpp"

#define TEST(expr, msg)          \
if (expr) std::cout << "PASSED TEST [" << msg << "]\n"; \
else std::cout << "FAILED TEST [" << msg << "]\n"; 

int main()
{
    std::size_t blk_total;
    _details::preprocess_sha512("abc", &blk_total);

}