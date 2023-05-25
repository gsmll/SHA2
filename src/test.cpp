#include <iostream>

#include "Hash.hpp"
#include "hash/SHA256.hpp"
#include "hash/SHA224.hpp"

#include "ProfileUtil.hpp"

int main()
{
    char *msg = new char[100001]{};
    Hash<256> hash;
    std::uint64_t accum = 0;
    for (std::size_t i = 0; i < 100000; ++i)
    {
        msg[i] = 'a';
        accum += chrono_time_it<std::chrono::nanoseconds>(sha256, msg);
    }
    delete[] msg;

    std::cout << (accum / 100000) << " ns\n";
}