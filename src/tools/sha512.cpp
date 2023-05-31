#include <iostream>
#include <string>
#include "hash/SHA512.hpp"

int main(int argc, char* argv[])
{
    Hash<512> hash;
    for (int i = 1; i < argc; ++i)
    {
        hash = sha512(argv[i]);
        std::cout << hash << "\n";
    }

    std::string s;
    while (std::cin >> s)
    {
        hash = sha512(s.c_str());
        std::cout << hash << "\n";
    }
}