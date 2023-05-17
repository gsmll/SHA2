#include <Hash.hpp>

#include <random>

int main()
{
    Hash<256> hash1, hash2;
    srand(time(NULL));
    int data[64];

    for (std::size_t i = 0; i < std::size(data); ++i)
    {
        data[i] = rand();
    }
    hash1.set(data);

    hash2 = hash1;

    std::cout << (hash1 == hash2) << "\n";
    
    for (std::size_t i = 0; i < std::size(data); ++i)
    {
        data[i] = rand();
    }
    hash2.set(data);

    std::cout << (hash1 < hash2) << "\n";
    std::cout << (hash1 > hash2) << "\n";

}