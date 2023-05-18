#include <bitset>

#include "Hash.hpp"
#include "hash/SHA256.hpp"

int main()
{
    std::cout << _details::general_sha256("abc") << "\n";
}