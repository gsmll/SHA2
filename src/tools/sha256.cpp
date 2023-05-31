#include <iostream>
#include <string>
#include "hash/SHA256.hpp"

int main(int argc, char* argv[])
{
    Hash<256> hash;
    for (int i = 1; i < argc; ++i)
    {
        hash = sha256(argv[i]);
        std::cout << hash << "\n";
    }

    std::string s;
    while (std::cin >> s)
    {
        hash = sha256(s.c_str());
        std::cout << hash << "\n";
    }
}