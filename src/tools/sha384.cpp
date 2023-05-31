#include <iostream>
#include <string>
#include "hash/SHA384.hpp"

int main(int argc, char* argv[])
{
    Hash<384> hash;
    for (int i = 1; i < argc; ++i)
    {
        hash = sha384(argv[i]);
        std::cout << hash << "\n";
    }

    std::string s;
    while (std::cin >> s)
    {
        hash = sha384(s.c_str());
        std::cout << hash << "\n";
    }
}