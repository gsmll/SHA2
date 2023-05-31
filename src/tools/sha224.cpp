#include <iostream>
#include <string>
#include "hash/SHA224.hpp"

int main(int argc, char* argv[])
{
    Hash<224> hash;
    for (int i = 1; i < argc; ++i)
    {
        hash = sha224(argv[i]);
        std::cout << hash << "\n";
    }

    std::string s;
    while (std::cin >> s)
    {
        hash = sha224(s.c_str());
        std::cout << hash << "\n";
    }

}