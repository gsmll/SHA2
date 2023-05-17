#ifndef HASH_HPP
#define HASH_HPP

#include <type_traits>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <iomanip>

template<std::size_t N, typename Enable = std::enable_if<N % 8 == 0>>
struct Hash
{
    static constexpr std::size_t Bytes = N / 8;

    std::uint8_t data[N / 8];

    Hash() : data{} {}
    Hash(const Hash&) = default
    Hash(Hash&&) = default
    Hash& operator=(const Hash&) = default
    Hash& operator(Hash&&) = default
    ~Hash() = default;

    template<typename T>
    Hash(T* buffer) : data{}
    {
        std::memcpy(data, buffer, N);
    }

    template<typename T>
    void set(T* buffer)
    {
        std::memcpy(data, buffer, N);
    }
};

template<std::size_t N>
std::ostream& operator<<(std::ostream& out, const Hash<N>& hash)
{
    auto flags = out.flags();
    out << std::hex << std::setw(2) << std::setfill('0');
    for (std::size_t i = 0; i < N / 8; ++i)
    {
        out << hash.data[i];
    }
    out.flags(flags);
    return out;
}

#endif