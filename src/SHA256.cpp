#include "hash/SHA256.hpp"

#include <cstring>

// #define DEBUG

#ifdef DEBUG
#include <iomanip>
#include <bitset>
#endif

#ifdef __AVX2__
#include "simd/SIMDUtil.hpp"
#endif

#define IMPLEMENTATION 1

namespace _details 
{

    [[nodiscard]] std::uint8_t* preprocess_sha256(const char* input, std::size_t* blk_total)
    {
        constexpr std::size_t BLOCK_BYTE_SIZE = 512 / 8;

        std::size_t strsize = std::strlen(input);
        *blk_total = (strsize + 8 + 1) / 64 + ( (strsize + 8 + 1) % 64 ? 1 : 0 );
        std::uint8_t* blks = new std::uint8_t[BLOCK_BYTE_SIZE * *blk_total]{ }; // paren important        
        std::memcpy(blks, input, strsize);
        blks[strsize] |= 1 << 7;
        std::uint64_t strbitsize = to_big_endian(8ul * strsize);
        std::memcpy(blks + *blk_total * BLOCK_BYTE_SIZE - sizeof(std::uint64_t), &strbitsize, sizeof(std::uint64_t));

#ifdef DEBUG
        std::cout << "PADDED MESSAGE\n" << std::hex << std::setfill('0');
/*         for (std::size_t i = 0; i < BLOCK_BYTE_SIZE * *blk_total; ++i)
        {
            std::cout << std::setw(2) << +blks[i];
            if (i % 4 == 3) std::cout << "\n";
        } */

        for (std::size_t i = 0; i < *blk_total * 512 / 32; ++i)
        {
            uint32_t val;
            std::memcpy(&val, blks + i * 4, 4);
            std::cout << std::bitset<32>{ val } << "\n";
        }
        std::cout << "\n" << std::dec << std::setfill(' ');
#endif

        return blks;
    }

    [[nodiscard]] Hash<256> general_sha256(const char* input)
    {
        using word = std::uint32_t;

        constexpr std::size_t bits_per_block = 512;
        constexpr std::size_t bytes_per_block = bits_per_block / 8;
        constexpr std::size_t rounds_per_chunk = 64;
        constexpr word round_values[64]{ 
            0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
            0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
            0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
            0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
            0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
            0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
            0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
            0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
        };

        word hash_data[8]{ 0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a, 0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19 };

        std::size_t blk_count;
        std::uint8_t* blks = preprocess_sha256(input, &blk_count);
        
        for (std::size_t chunk_idx = 0; chunk_idx < blk_count; ++chunk_idx)
        {
            uint32_t temp0_, temp1_;

            // generate message schedule array
            word msg_schedule_arr[64];
            for (std::size_t i = 0; i < 16; ++i) 
            {
                std::memcpy(&temp0_, blks + chunk_idx * bytes_per_block + i * sizeof(word), sizeof(word));
                msg_schedule_arr[i] = to_big_endian(temp0_);
            }
            for (std::size_t i = 16; i < rounds_per_chunk; ++i)
            {
                temp0_ = rotr(msg_schedule_arr[i - 15], 7) ^ 
                         rotr(msg_schedule_arr[i - 15], 18) ^
                         (msg_schedule_arr[i - 15] >> 3);
                temp1_ = rotr(msg_schedule_arr[i - 2], 17) ^
                         rotr(msg_schedule_arr[i - 2], 19) ^
                         (msg_schedule_arr[i - 2] >> 10);
                msg_schedule_arr[i] = msg_schedule_arr[i - 16] + temp0_ + msg_schedule_arr[i - 7] + temp1_;
            }

#ifdef DEBUG
            std::cout << "MESSAGE SCHEDULE ARRAY \n" << std::setfill('0') << std::hex;
            for (std::size_t i = 0; i < rounds_per_chunk; ++i)
            {
                // std::cout << std::bitset<32>{ msg_schedule_arr[i] } << "\n";
                std::cout << std::setw(8) << msg_schedule_arr[i] << "\n";
            }
            std::cout << "\n" << std::setfill(' ') << std::dec;
#endif
            // set rotatable temp hashes
            // 0 1 2 3 4 5 6 7
            // a b c d e f g h

            word s1, s2, ch, maj;
            word hash_buffer[8 + rounds_per_chunk];
            word* temp_hash = hash_buffer + rounds_per_chunk;
            std::memcpy(temp_hash, hash_data, 8 * sizeof(word));

            for (std::size_t i = 0; i < rounds_per_chunk; ++i, --temp_hash)
            {
                s1 = rotr(temp_hash[4], 6) ^ rotr(temp_hash[4], 11) ^ rotr(temp_hash[4], 25);
                ch = (temp_hash[4] & temp_hash[5]) ^ (~temp_hash[4] & temp_hash[6]);
                temp0_ = temp_hash[7] + s1 + ch + round_values[i] + msg_schedule_arr[i];
                s2 = rotr(temp_hash[0], 2) ^ rotr(temp_hash[0], 13) ^ rotr(temp_hash[0], 22);
                maj = (temp_hash[0] & temp_hash[1]) ^ (temp_hash[0] & temp_hash[2]) ^ (temp_hash[1] & temp_hash[2]);
                temp1_ = s2 + maj;

                temp_hash[-1] = temp0_ + temp1_;
                temp_hash[3] += temp0_;

#ifdef DEBUG
                std::cout << "SCHEDULE\n";
                std::cout << std::hex << std::setfill('0');
                for (std::size_t w = 0; w < 8; ++w)
                {
                    std::cout << std::setw(8) << temp_hash[w] << " ";
                }
                std::cout << "\n\t" << "temp0_ = " << std::bitset<32>{ temp0_ } << " | temp1_ = " << std::bitset<32>{ temp1_ } << "\n";

                std::cout << std::dec << std::setfill(' ');
#endif
            }

            for (std::size_t i = 0; i < 8; ++i)
            {
                hash_data[i] += temp_hash[i];
            }
        }

        // flip back if necessary
        for (std::size_t i = 0; i < 8; ++i)
        {
            hash_data[i] = to_big_endian(hash_data[i]);
        }

        delete[] blks;
        return { hash_data };
    }

/*
#ifdef __AVX2__

    INLINE static __m256i sigma0(__m256i input)
    {
        __m256i _a = simd::rotr(input, 7);
        __m256i _b = simd::rotr(input, 18);
        __m256i _c = _mm256_srli_epi32(input, 3);

        return _mm256_xor_si256( _a, _mm256_xor_si256( _b, _c ) );
    }

    INLINE static __m64 sigma1(__m64 input)
    {
        __m64 _a = simd::rotr(input, 17);
        __m64 _b = simd::rotr(input, 19);
        __m64 _c = _mm_srli_pi32(input, 10);

        return _mm_xor_si64( _a, _mm_xor_si64( _b, _c ) );
    }

    [[nodiscard]] Hash<256> simd_sha256(const char* input)
    {
        using word = std::uint32_t;

        constexpr std::size_t bits_per_block = 512;
        constexpr std::size_t bytes_per_block = bits_per_block / 8;
        constexpr std::size_t rounds_per_chunk = 64;
        constexpr word round_values[64]{ 
            0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
            0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
            0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
            0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
            0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
            0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
            0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
            0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
        };

        word hash_data[8]{ 0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a, 0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19 };

        std::size_t blk_count;
        std::uint8_t* blks = preprocess_sha256(input, &blk_count);

        for (std::size_t chunk_idx = 0; chunk_idx < blk_count; ++chunk_idx)
        {
            word temp0_, temp1_;
            word msg_schedule_arr[64];
            for (std::size_t i = 0; i < 16; ++i) 
            {
                std::memcpy(&temp0_, blks + chunk_idx * bytes_per_block + i * sizeof(word), sizeof(word));
                msg_schedule_arr[i] = to_big_endian(temp0_);
            }

            // word w7_lo256_buffer[4];
            alignas(32) word simd_buffer[8];
            for (std::size_t i = 2; i < rounds_per_chunk / 8; ++i)
            {
                word* ptr = msg_schedule_arr + i * 8; 

                __m256i w16 = _mm256_loadu_si256((__m256i*) (ptr - 16));
                __m256i w15 = _mm256_loadu_si256((__m256i*) (ptr - 15));
                __m256i s0 = sigma0(w15);
                _mm256_store_si256((__m256i*) simd_buffer, s0);

                for (std::size_t k = 0; k < 8; ++k)
                {
                    std::size_t index = i * 8 + k;
                    temp1_ = rotr(msg_schedule_arr[index - 2], 17) ^
                        rotr(msg_schedule_arr[index - 2], 19) ^
                        (msg_schedule_arr[index - 2] >> 10);
                    msg_schedule_arr[index] = simd_buffer[k] + msg_schedule_arr[index - 7] + temp1_;
                }

#ifdef DEBUG
                word __temp_word[8];
                _mm256_storeu_si256((__m256i_u*) __temp_word, s0);
                std::cout << "s0 = " << std::bitset<32>{ __temp_word[0] } << "\n";
#endif
                
                __m256i s0_w16 = _mm256_add_epi32(w16, s0);

                __m128i lo256, hi256;
                _mm256_storeu2_m128i(&lo256, &hi256, s0_w16);

                __m128i w7 = _mm_loadu_si128((__m128i*) (ptr - 7));
                __m128i w7_lo256 = _mm_add_epi32(w7, lo256);
                // _mm_storeu_epi32(w7_lo256_buffer, w7_lo256);

                __m64 lo64, hi64;
                _mm_storel_pi(&lo64, _mm_castsi128_ps(w7_lo256));
                _mm_storeh_pi(&hi64, _mm_castsi128_ps(w7_lo256));

                __m64 w2;
                std::uint64_t w2val;
                std::memcpy(&w2, ptr - 2, 8);
                __m64 s1 = sigma1(w2);

#ifdef DEBUG
                word __2_temp[2];
                std::memcpy(__2_temp, &w2, 8);
                std::cout << "s1 = " << std::bitset<32>{ __2_temp[0] } << "\n";
#endif

                __m64 out[4];

                out[0] = _mm_add_pi32(s1, lo64);
#ifdef DEBUG
                std::memcpy(__2_temp, &out[0], 8);
                std::cout << "w = " << std::bitset<32>{ __2_temp[0] } << "\n";
#endif
                s1 = sigma1(out[0]);
                out[1] = _mm_add_pi32(s1, hi64);

                __m128i w3 = _mm_loadu_si128((__m128i*) (ptr - 3));
                __m128i w3_hi256 = _mm_add_epi32(w3, hi256);

                _mm_storel_pi(&lo64, _mm_castsi128_ps(w3_hi256));
                _mm_storeh_pi(&hi64, _mm_castsi128_ps(w3_hi256));        

                s1 = sigma1(out[1]);
                out[2] = _mm_add_pi32(s1, lo64);
                s1 = sigma1(out[2]);
                out[3] = _mm_add_pi32(s1, hi64);

                __m256i words = _mm256_loadu_si256((__m256i*) out);
                _mm256_storeu_si256((__m256i_u*) ptr, words);  
   
            }

#ifdef DEBUG
            std::cout << "MESSAGE SCHEDULE ARRAY \n" << std::setfill('0') << std::hex;
            for (std::size_t i = 0; i < rounds_per_chunk; ++i)
            {
                std::cout << std::bitset<32>{ msg_schedule_arr[i] } << "\n";
                // std::cout << std::setw(8) << msg_schedule_arr[i] << "\n";
            }
            std::cout << "\n" << std::setfill(' ') << std::dec;
#endif
            // set rotatable temp hashes
            // 0 1 2 3 4 5 6 7
            // a b c d e f g h

            word s1, s2, ch, maj;
            word hash_buffer[8 + rounds_per_chunk];
            word* temp_hash = hash_buffer + rounds_per_chunk;
            std::memcpy(temp_hash, hash_data, 8 * sizeof(word));

            for (std::size_t i = 0; i < rounds_per_chunk; ++i, --temp_hash)
            {
                s1 = rotr(temp_hash[4], 6) ^ rotr(temp_hash[4], 11) ^ rotr(temp_hash[4], 25);
                ch = (temp_hash[4] & temp_hash[5]) ^ (~temp_hash[4] & temp_hash[6]);
                temp0_ = temp_hash[7] + s1 + ch + round_values[i] + msg_schedule_arr[i];
                s2 = rotr(temp_hash[0], 2) ^ rotr(temp_hash[0], 13) ^ rotr(temp_hash[0], 22);
                maj = (temp_hash[0] & temp_hash[1]) ^ (temp_hash[0] & temp_hash[2]) ^ (temp_hash[1] & temp_hash[2]);
                temp1_ = s2 + maj;

                temp_hash[-1] = temp0_ + temp1_;
                temp_hash[3] += temp0_;

#ifdef DEBUG
                std::cout << "SCHEDULE\n";
                std::cout << std::hex << std::setfill('0');
                for (std::size_t w = 0; w < 8; ++w)
                {
                    std::cout << std::setw(8) << temp_hash[w-1] << " ";
                }
                std::cout << "\n\t" << "temp0_ = " << std::bitset<32>{ temp0_ } << " | temp1_ = " << std::bitset<32>{ temp1_ } << "\n";

                std::cout << std::dec << std::setfill(' ');
#endif
            }

            for (std::size_t i = 0; i < 8; ++i)
            {
                hash_data[i] += temp_hash[i];
            }
        }

        // flip back if necessary
        for (std::size_t i = 0; i < 8; ++i)
        {
            hash_data[i] = to_big_endian(hash_data[i]);
        }

        delete[] blks;
        return { hash_data };
    }
#endif
*/

}

[[nodiscard]] Hash<256> sha256(const char* input)
{
/* #ifdef __SHA__
    return _details::instruction_sha256(input);
#if __AVX2__
    return _details::simd_sha256(input);
#else */
    return _details::general_sha256(input);
// #endif
}