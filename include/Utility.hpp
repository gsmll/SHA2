#ifndef UTILITY
#define UTILITY

#include <cstdint>

// Architecture specific

#if defined(__x86_64__) || defined(__i386__) // x86 Arch

#ifdef __OPTIMIZE__
    #include <immintrin.h>
    #define INLINE inline __attribute__((__always_inline__))
#else
    #define __OPTIMIZE__
    #include <immintrin.h>
    #define INLINE inline
    #undef __OPTIMIZE__
#endif

INLINE auto rotl(std::uint32_t val, int d)
{
    return _rotl(val, d);
}

INLINE auto rotr(std::uint32_t val, int d)
{
    return _rotr(val, d);
}

#else // ARM Arch

INLINE auto rotl(std::uint32_t val, int d)
{
    d &= 31;
    return (val << d) | (val >> (-d & 31));
}

INLINE auto rotr(std::uint32_t val, int d)
{
    d &= 31;
    return (val >> d) | (val << (-d & 31));
}

#endif

// Not architecture specific

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__

INLINE auto to_big_endian(std::uint32_t val) { return __builtin_bswap32(val); }
INLINE auto to_big_endian(std::uint64_t val) { return __builtin_bswap64(val); }
INLINE auto to_little_endian(std::uint32_t val) { return val; }
INLINE auto to_little_endian(std::uint64_t val) { return val; }

#else

INLINE auto to_big_endian(std::uint32_t val) { return val; }
INLINE auto to_big_endian(std::uint64_t val) { return val; }
INLINE auto to_little_endian(std::uint32_t val) { return __builtin_bswap32(val); }
INLINE auto to_little_endian(std::uint64_t val) { return __builtin_bswap64(val); }

#endif

#endif