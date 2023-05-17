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
    Hash(const Hash&) = default;
    Hash(Hash&&) = default;
    Hash& operator=(const Hash&) = default;
    Hash& operator=(Hash&&) = default;
    ~Hash() = default;

    template<typename T>
    Hash(T* buffer) : data{}
    {
        std::memcpy(data, buffer, Bytes);
    }

    template<typename T>
    void set(T* buffer)
    {
        std::memcpy(data, buffer, Bytes);
    }
};

template<std::size_t N>
bool operator<(Hash<N>& a, Hash<N>& b)
{
    return std::memcmp(a.data, b.data, Hash<N>::Bytes) < 0;
}

template<std::size_t N>
bool operator>(Hash<N>& a, Hash<N>& b)
{
    return std::memcmp(a.data, b.data, Hash<N>::Bytes) > 0;
}

template<std::size_t N>
bool operator==(Hash<N>& a, Hash<N>& b)
{
    return std::memcmp(a.data, b.data, Hash<N>::Bytes) == 0;
}

template<std::size_t N>
bool operator!=(Hash<N>& a, Hash<N>& b)
{
    return !(a == b);
}

template<std::size_t N>
bool operator<=(Hash<N>& a, Hash<N>& b)
{
    return !(a > b);
}

template<std::size_t N>
bool operator>=(Hash<N>& a, Hash<N>& b)
{
    return !(a < b);
}

template<std::size_t N>
std::ostream& operator<<(std::ostream& out, const Hash<N>& hash)
{
    auto flags = out.flags();
    out << std::hex << std::setfill('0');
    for (std::size_t i = 0; i < Hash<N>::Bytes; ++i)
    {
        out << std::setw(2) << static_cast<int>(hash.data[i]);
    }
    out.flags(flags);
    return out;
}

#endif