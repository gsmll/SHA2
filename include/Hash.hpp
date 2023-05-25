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

    template<std::size_t K, typename EnableT = std::enable_if<(K > N)>>
    explicit Hash(const Hash<K>& a)
    {
        std::memcpy(data, a.data, Bytes);
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
bool operator<(Hash<N>&& a, Hash<N>&& b)
{
    return std::memcmp(a.data, b.data, Hash<N>::Bytes) < 0;
}

template<std::size_t N>
bool operator>(Hash<N>&& a, Hash<N>&& b)
{
    return std::memcmp(a.data, b.data, Hash<N>::Bytes) > 0;
}

template<std::size_t N>
bool operator==(Hash<N>&& a, Hash<N>&& b)
{
    return std::memcmp(a.data, b.data, Hash<N>::Bytes) == 0;
}

template<std::size_t N>
bool operator!=(Hash<N>&& a, Hash<N>&& b)
{
    return !(a == b);
}

template<std::size_t N>
bool operator<=(Hash<N>&& a, Hash<N>&& b)
{
    return !(a > b);
}

template<std::size_t N>
bool operator>=(Hash<N>&& a, Hash<N>&& b)
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

namespace _details
{
    template<char... C>
    struct is_valid_hex : public std::false_type {};

    template<char... C>
    struct is_valid_hex<'0', 'x', C...> : public std::true_type {};

    template<char... C>
    constexpr inline bool is_valid_hex_v = is_valid_hex<C...>::value;

    template<char... C>
    using enable_hash_literal_t = std::enable_if_t<(sizeof...(C) > 2) && sizeof...(C) % 2 == 0 && is_valid_hex_v<C...>, Hash<(sizeof...(C) - 2) * 4>>;

    constexpr int hex_char_to_int(char c)
    {
        if (c >= 'a' && c <= 'f') return static_cast<int>(c) - static_cast<int>('a') + 10;
        else if (c >= 'A' && c <= 'F') return static_cast<int>(c) - static_cast<int>('A') + 10;
        return static_cast<int>(c) - static_cast<int>('0');
    }

    template<std::size_t N>
    Hash<(N - 2) * 4> hex_to_hash(const char (&hex)[N])
    {
        std::uint8_t data[(N - 2) / 2];

        for (std::size_t i = 1; i < N / 2; ++i)
        {
            data[i - 1] = static_cast<std::uint8_t>( 16 * hex_char_to_int(hex[2 * i]) + hex_char_to_int(hex[2 * i + 1]) );
        }

        return data;
    }
}

template<char... C>
_details::enable_hash_literal_t<C...> operator""_hash()
{
    return _details::hex_to_hash({ C... });
}

template<std::size_t N>
Hash<N> str_to_hash(const char* cstr)
{
    std::size_t str_size = strlen(cstr);
    std::size_t min = std::min(str_size / 2, N / 2);
    std::uint8_t data[N / 8];   
    for (std::size_t i = 0; i < min; ++i)
    {   
        data[i] = static_cast<std::uint8_t>(16 * _details::hex_char_to_int(cstr[2 * i])) + _details::hex_char_to_int(cstr[2 * i + 1]);
    }

    Hash<N> hash{};
    std::memcpy(hash.data, data, min);
    return hash;
}

#endif