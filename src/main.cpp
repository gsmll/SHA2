#include <Hash.hpp>

#include <random>

int main()
{
    Hash<256> hash;
    srand(time(NULL));
    int data[64];

    for (int k = 0; k < 100; ++k)
    {
        for (std::size_t i = 0; i < std::size(data); ++i)
        {
            data[i] = rand();
        }
        hash.set(data);
        std::cout << hash << "\n";
    }
}