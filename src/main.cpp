#include <bitset>

#include "Hash.hpp"
#include "hash/SHA256.hpp"

int main()
{
    // std::cout << _details::general_sha256("abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq") << "\n";

    char* buffer = new char[1000001];
    buffer[1000000] = '\0';
    for (std::size_t i = 0; i < 1000000; ++i)
    {
        buffer[i] = 'a';
    }

    std::cout << _details::general_sha256(buffer) << "\n";

    delete[] buffer;
}