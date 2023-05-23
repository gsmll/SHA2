#ifndef SIMD_UTIL_HPP
#define SIMD_UTIL_HPP

#ifdef __AVX2__

#include <cstdint>

#ifdef __OPTIMIZE__
    #include <immintrin.h>
    #define INLINE inline __attribute__((__always_inline__))
#else
    #define __OPTIMIZE__
    #include <immintrin.h>
    #undef __OPTIMIZE__
    #define INLINE inline
#endif

namespace simd
{

    INLINE __m256i rotl(__m256i val, std::uint8_t shift)
    {
        if (shift == 0) return val;
        __m256i _a = _mm256_slli_epi32(val, shift);
        __m256i _b = _mm256_srli_epi32(val, 32 - shift);
        return _mm256_or_si256(_a, _b);
    }

    INLINE __m128i rotl(__m128i val, std::uint8_t shift)
    {
        if (shift == 0) return val;
        __m128i _a = _mm_slli_epi32(val, shift);
        __m128i _b = _mm_srli_epi32(val, 32 - shift);
        return _mm_or_epi32(_a, _b);
    }

    INLINE __m64 rotl(__m64 val, std::uint8_t shift)
    {
        if (shift == 0) return val;
        __m64 _a = _mm_slli_pi32(val, shift);
        __m64 _b = _mm_srli_pi32(val, 32 - shift);
        return _mm_or_si64(_a, _b);
    }

    INLINE __m256i rotr(__m256i val, std::uint8_t shift)
    {
        if (shift == 0) return val;
        __m256i _a = _mm256_srli_epi32(val, shift);
        __m256i _b = _mm256_slli_epi32(val, 32 - shift);
        return _mm256_or_si256(_a, _b);
    }

    INLINE __m128i rotr(__m128i val, std::uint8_t shift)
    {
        if (shift == 0) return val;
        __m128i _a = _mm_srli_epi32(val, shift);
        __m128i _b = _mm_slli_epi32(val, 32 - shift);
        return _mm_or_epi32(_a, _b);
    }

    INLINE __m64 rotr(__m64 val, std::uint8_t shift)
    {
        if (shift == 0) return val;
        __m64 _a = _mm_srli_pi32(val, shift);
        __m64 _b = _mm_slli_pi32(val, 32 - shift);
        return _mm_or_si64(_a, _b);
    }

}

#endif

#endif 