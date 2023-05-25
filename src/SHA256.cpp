#include "hash/SHA256.hpp"

#include <cstring>
#define DEBUG true

// #define DEBUG

#ifdef DEBUG
#include <iomanip>
#include <bitset>
#endif

#ifdef __AVX2__
#include "simd/SIMDUtil.hpp"
#endif

#define LLVM_MCA_BEGIN __asm volatile("# LLVM-MCA-BEGIN");
#define LLVM_MCA_END __asm volatile("# LLVM-MCA-END");

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

/* #ifdef DEBUG
        std::cout << "PADDED MESSAGE\n" << std::hex << std::setfill('0');
        for (std::size_t i = 0; i < *blk_total * 512 / 32; ++i)
        {
            uint32_t val;
            std::memcpy(&val, blks + i * 4, 4);
            std::cout << std::bitset<32>{ val } << "\n";
        }
        std::cout << "\n" << std::dec << std::setfill(' ');
#endif */

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
                // LLVM_MCA_BEGIN

                temp0_ = rotr(msg_schedule_arr[i - 15], 7) ^ 
                         rotr(msg_schedule_arr[i - 15], 18) ^
                         (msg_schedule_arr[i - 15] >> 3);
                temp1_ = rotr(msg_schedule_arr[i - 2], 17) ^
                         rotr(msg_schedule_arr[i - 2], 19) ^
                         (msg_schedule_arr[i - 2] >> 10);
                msg_schedule_arr[i] = msg_schedule_arr[i - 16] + temp0_ + msg_schedule_arr[i - 7] + temp1_;

                // LLVM_MCA_END
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

#ifdef __AVX2__

    INLINE static __m256i sigma0(__m256i input)
    {
        __m256i _a = simd::rotr(input, 7);
        __m256i _b = simd::rotr(input, 18);
        __m256i _c = _mm256_srli_epi32(input, 3);

        __m256i _xor_bc = _mm256_xor_si256( _b, _c );

        return _mm256_xor_si256( _a, _xor_bc );
    }

    INLINE static std::uint32_t sigma1(std::uint32_t input)
    {
        return rotr(input, 17) ^ rotr(input, 19) ^ (input >> 10);
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
            word msg_schedule_arr[64]{};
            for (std::size_t i = 0; i < 16; ++i) 
            {
                std::memcpy(&temp0_, blks + chunk_idx * bytes_per_block + i * sizeof(word), sizeof(word));
                msg_schedule_arr[i] = to_big_endian(temp0_);
            }

            __m256i a, b, c, d;
            word* ptr;
            for (std::size_t i = 2; i < rounds_per_chunk / 8; ++i)
            {
                // LLVM_MCA_BEGIN

                ptr = msg_schedule_arr + i * 8; 

                a = _mm256_loadu_si256((__m256i_u*) (ptr - 16));
                b = _mm256_loadu_si256((__m256i_u*) (ptr - 15));
                c = _mm256_loadu_si256((__m256i_u*) (ptr - 7));

                d = sigma0(b);
                b = _mm256_add_epi32(d, c);
                c = _mm256_add_epi32(a, b); // have to correct last value;

                _mm256_storeu_si256((__m256i_u*) ptr, c);

                ptr[0] += sigma1(ptr[-2]);
                ptr[2] += sigma1(ptr[0]);
                ptr[4] += sigma1(ptr[2]);
                ptr[6] += sigma1(ptr[4]);

                ptr[1] += sigma1(ptr[-1]);
                ptr[3] += sigma1(ptr[1]);
                ptr[5] += sigma1(ptr[3]);
                ptr[7] += sigma1(ptr[5]) + ptr[0];

                // LLVM_MCA_END
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

#ifdef __SHA__

#ifdef DEBUG
    #define DEBUG_ABCDEFGH(abef, cdgh)                              \
    _mm_storeu_si128((__m128i_u*) words, abef);                     \
    _mm_storeu_si128((__m128i_u*) (words + 4), cdgh);               \
    std::cout << std::setfill('0');                                 \
    std::cout << std::hex << std::setw(8) << words[3] << " "        \
                          << std::setw(8) << words[2] << " "        \
                          << std::setw(8) << words[7] << " "        \
                          << std::setw(8) << words[6] << " "        \
                          << std::setw(8) << words[1] << " "        \
                          << std::setw(8) << words[0] << " "        \
                          << std::setw(8) << words[5] << " "        \
                          << std::setw(8) << words[4] << "\n";      \
    std::cout << std::dec << std::setfill(' ')
#else 
    #define DEBUG_ABCDEFGH(abef, cdgh)
#endif

    [[nodiscard]] Hash<256> instruction_sha256(const char* input)
    {
        using word = std::uint32_t;

        constexpr std::size_t bits_per_block = 512;
        constexpr std::size_t bytes_per_block = bits_per_block / 8;

        std::size_t blk_count;
        std::uint8_t* blks = preprocess_sha256(input, &blk_count);

        __m128i hash_CDGH = _mm_set_epi32(0x3c6ef372, 0xa54ff53a, 0x1f83d9ab, 0x5be0cd19);
        __m128i hash_ABEF = _mm_set_epi32(0x6a09e667, 0xbb67ae85, 0x510e527f, 0x9b05688c); 

        std::uint8_t* ptr = blks;
        __m128i CDGH, ABEF;
        __m128i msg, _msg0, _msg1, _msg2, _msg3; // Every 16 words, with 4 rounds each, requires 4 _msg variables
        __m128i t_0; // for W[t-7] in adding values in message schedule queue
        __m128i big_endian_flip_mask = _mm_set_epi8(12, 13, 14, 15, 8, 9, 10, 11, 4, 5, 6, 7, 0, 1, 2, 3);

#ifdef DEBUG
        word words[8];
#endif

        for (std::size_t chunk_idx = 0; chunk_idx < blk_count; ++chunk_idx, ptr += bytes_per_block)
        {
            CDGH = hash_CDGH;
            ABEF = hash_ABEF;

            // round 0-3
            _msg0 = _mm_loadu_si128((__m128i_u*) ptr);
            _msg0 = _mm_shuffle_epi8(_msg0, big_endian_flip_mask); // flip to W19, W18, W17, W16
            msg = _mm_add_epi32(_msg0, _mm_set_epi32(0xe9b5dba5, 0xb5c0fbcf, 0x71374491, 0x428a2f98)); // add to M19, M18, M17, M16
            CDGH = _mm_sha256rnds2_epu32(CDGH, ABEF, msg);
            DEBUG_ABCDEFGH(CDGH, ABEF);
            msg = _mm_shuffle_epi32(msg, 0b00001110); // flip 64 bytes [ 0 0 3 2 ]
            ABEF = _mm_sha256rnds2_epu32(ABEF, CDGH, msg);
            DEBUG_ABCDEFGH(ABEF, CDGH);

            // round 4-7
            _msg1 = _mm_loadu_si128((__m128i_u*) (ptr + 1 * sizeof(word) * 4));
            _msg1 = _mm_shuffle_epi8(_msg1, big_endian_flip_mask); 
            msg = _mm_add_epi32(_msg1, _mm_set_epi32(0xab1c5ed5, 0x923f82a4, 0x59f111f1, 0x3956c25b));
            CDGH = _mm_sha256rnds2_epu32(CDGH, ABEF, msg);
            DEBUG_ABCDEFGH(CDGH, ABEF);
            msg = _mm_shuffle_epi32(msg, 0b00001110); // flip 64 bytes
            ABEF = _mm_sha256rnds2_epu32(ABEF, CDGH, msg);
            DEBUG_ABCDEFGH(ABEF, CDGH);

            // round 8 - 11
            _msg2 = _mm_loadu_si128((__m128i_u*) (ptr + 2 * sizeof(word) * 4));
            _msg2 = _mm_shuffle_epi8(_msg2, big_endian_flip_mask); 
            msg = _mm_add_epi32(_msg2, _mm_set_epi32(0x550c7dc3, 0x243185be, 0x12835b01, 0xd807aa98));
            CDGH = _mm_sha256rnds2_epu32(CDGH, ABEF, msg);
            DEBUG_ABCDEFGH(CDGH, ABEF);
            msg = _mm_shuffle_epi32(msg, 0b00001110); // flip 64 bytes
            ABEF = _mm_sha256rnds2_epu32(ABEF, CDGH, msg);
            DEBUG_ABCDEFGH(ABEF, CDGH);

            _msg0 = _mm_sha256msg1_epu32(_msg0, _msg1); // SIGMA0 for RUN 16

            // round 12 - 15
            _msg3 = _mm_loadu_si128((__m128i_u*) (ptr + 3 * sizeof(word) * 4));
            _msg3 = _mm_shuffle_epi8(_msg3, big_endian_flip_mask); 
            msg = _mm_add_epi32(_msg3, _mm_set_epi32(0xc19bf174, 0x9bdc06a7, 0x80deb1fe, 0x72be5d74));
            CDGH = _mm_sha256rnds2_epu32(CDGH, ABEF, msg);
            DEBUG_ABCDEFGH(CDGH, ABEF);
            msg = _mm_shuffle_epi32(msg, 0b00001110); // flip 64 bytes
            ABEF = _mm_sha256rnds2_epu32(ABEF, CDGH, msg);
            DEBUG_ABCDEFGH(ABEF, CDGH);

            t_0 = _mm_alignr_epi8(_msg3, _msg2, 4); // W[t-7] for RUN 16
            _msg0 = _mm_add_epi32(_msg0, t_0); // W[t-16] + sigma0 + W[t-7] 
            _msg1 = _mm_sha256msg1_epu32(_msg1, _msg2); // SIGMA0 for RUN 20

            // begin calculating messages

            // round 16 - 19
            _msg0 = _mm_sha256msg2_epu32(_msg0, _msg3);
            msg = _mm_add_epi32(_msg0, _mm_set_epi32(0x240ca1cc, 0x0fc19dc6, 0xefbe4786, 0xe49b69c1));
            CDGH = _mm_sha256rnds2_epu32(CDGH, ABEF, msg);
            DEBUG_ABCDEFGH(CDGH, ABEF);
            msg = _mm_shuffle_epi32(msg, 0b00001110); // flip 64 bytes
            ABEF = _mm_sha256rnds2_epu32(ABEF, CDGH, msg);
            DEBUG_ABCDEFGH(ABEF, CDGH);

            t_0 = _mm_alignr_epi8(_msg0, _msg3, 4); // W[t-7] for RUN 20
            _msg1 = _mm_add_epi32(_msg1, t_0); // W[t-16] + sigma0 + W[t-7] 
            _msg2 = _mm_sha256msg1_epu32(_msg2, _msg3); // SIGMA0 for RUN 24

            // round 20 - 23
            _msg1 = _mm_sha256msg2_epu32(_msg1, _msg0);
            msg = _mm_add_epi32(_msg1, _mm_set_epi32(0x76f988da, 0x5cb0a9dc, 0x4a7484aa, 0x2de92c6f));
            CDGH = _mm_sha256rnds2_epu32(CDGH, ABEF, msg);
            DEBUG_ABCDEFGH(CDGH, ABEF);
            msg = _mm_shuffle_epi32(msg, 0b00001110); // flip 64 bytes
            ABEF = _mm_sha256rnds2_epu32(ABEF, CDGH, msg);
            DEBUG_ABCDEFGH(ABEF, CDGH);

            t_0 = _mm_alignr_epi8(_msg1, _msg0, 4); // W[t-7] for RUN 24
            _msg2 = _mm_add_epi32(_msg2, t_0); // W[t-16] + sigma0 + W[t-7] 
            _msg3 = _mm_sha256msg1_epu32(_msg3, _msg0); // SIGMA0 for RUN 28

            // round 24 - 27
            _msg2 = _mm_sha256msg2_epu32(_msg2, _msg1);
            msg = _mm_add_epi32(_msg2, _mm_set_epi32(0xbf597fc7, 0xb00327c8, 0xa831c66d, 0x983e5152));
            CDGH = _mm_sha256rnds2_epu32(CDGH, ABEF, msg);
            DEBUG_ABCDEFGH(CDGH, ABEF);
            msg = _mm_shuffle_epi32(msg, 0b00001110); // flip 64 bytes
            ABEF = _mm_sha256rnds2_epu32(ABEF, CDGH, msg);
            DEBUG_ABCDEFGH(ABEF, CDGH);

            t_0 = _mm_alignr_epi8(_msg2, _msg1, 4); // W[t-7] for RUN 28
            _msg3 = _mm_add_epi32(_msg3, t_0); // W[t-16] + sigma0 + W[t-7] 
            _msg0 = _mm_sha256msg1_epu32(_msg0, _msg1); // SIGMA0 for RUN 32

            // round 28 - 31
            _msg3 = _mm_sha256msg2_epu32(_msg3, _msg2);
            msg = _mm_add_epi32(_msg3, _mm_set_epi32(0x14292967, 0x06ca6351, 0xd5a79147, 0xc6e00bf3));
            CDGH = _mm_sha256rnds2_epu32(CDGH, ABEF, msg);
            DEBUG_ABCDEFGH(CDGH, ABEF);
            msg = _mm_shuffle_epi32(msg, 0b00001110); // flip 64 bytes
            ABEF = _mm_sha256rnds2_epu32(ABEF, CDGH, msg);
            DEBUG_ABCDEFGH(ABEF, CDGH);

            t_0 = _mm_alignr_epi8(_msg3, _msg2, 4); // W[t-7] for RUN 32
            _msg0 = _mm_add_epi32(_msg0, t_0); // W[t-16] + sigma0 + W[t-7] 
            _msg1 = _mm_sha256msg1_epu32(_msg1, _msg2); // SIGMA0 for RUN 36

            // round 32 - 35
            _msg0 = _mm_sha256msg2_epu32(_msg0, _msg3);
            msg = _mm_add_epi32(_msg0, _mm_set_epi32(0x53380d13, 0x4d2c6dfc, 0x2e1b2138, 0x27b70a85));
            CDGH = _mm_sha256rnds2_epu32(CDGH, ABEF, msg);
            DEBUG_ABCDEFGH(CDGH, ABEF);
            msg = _mm_shuffle_epi32(msg, 0b00001110); // flip 64 bytes
            ABEF = _mm_sha256rnds2_epu32(ABEF, CDGH, msg);
            DEBUG_ABCDEFGH(ABEF, CDGH);

            t_0 = _mm_alignr_epi8(_msg0, _msg3, 4); // W[t-7] for RUN 36
            _msg1 = _mm_add_epi32(_msg1, t_0); // W[t-16] + sigma0 + W[t-7] 
            _msg2 = _mm_sha256msg1_epu32(_msg2, _msg3); // SIGMA0 for RUN 40

            // round 36 - 39
            _msg1 = _mm_sha256msg2_epu32(_msg1, _msg0);
            msg = _mm_add_epi32(_msg1, _mm_set_epi32(0x92722c85, 0x81c2c92e, 0x766a0abb, 0x650a7354));
            CDGH = _mm_sha256rnds2_epu32(CDGH, ABEF, msg);
            DEBUG_ABCDEFGH(CDGH, ABEF);
            msg = _mm_shuffle_epi32(msg, 0b00001110); // flip 64 bytes
            ABEF = _mm_sha256rnds2_epu32(ABEF, CDGH, msg);
            DEBUG_ABCDEFGH(ABEF, CDGH);

            t_0 = _mm_alignr_epi8(_msg1, _msg0, 4); // W[t-7] for RUN 40
            _msg2 = _mm_add_epi32(_msg2, t_0); // W[t-16] + sigma0 + W[t-7] 
            _msg3 = _mm_sha256msg1_epu32(_msg3, _msg0); // SIGMA0 for RUN 44

            // round 40 - 43
            _msg2 = _mm_sha256msg2_epu32(_msg2, _msg1);
            msg = _mm_add_epi32(_msg2, _mm_set_epi32(0xc76c51a3, 0xc24b8b70, 0xa81a664b, 0xa2bfe8a1));
            CDGH = _mm_sha256rnds2_epu32(CDGH, ABEF, msg);
            DEBUG_ABCDEFGH(CDGH, ABEF);
            msg = _mm_shuffle_epi32(msg, 0b00001110); // flip 64 bytes
            ABEF = _mm_sha256rnds2_epu32(ABEF, CDGH, msg);
            DEBUG_ABCDEFGH(ABEF, CDGH);

            t_0 = _mm_alignr_epi8(_msg2, _msg1, 4); // W[t-7] for RUN 44
            _msg3 = _mm_add_epi32(_msg3, t_0); // W[t-16] + sigma0 + W[t-7] 
            _msg0 = _mm_sha256msg1_epu32(_msg0, _msg1); // SIGMA0 for RUN 48

            // round 44 - 47
            _msg3 = _mm_sha256msg2_epu32(_msg3, _msg2);
            msg = _mm_add_epi32(_msg3, _mm_set_epi32(0x106aa070, 0xf40e3585, 0xd6990624, 0xd192e819));
            CDGH = _mm_sha256rnds2_epu32(CDGH, ABEF, msg);
            DEBUG_ABCDEFGH(CDGH, ABEF);
            msg = _mm_shuffle_epi32(msg, 0b00001110); // flip 64 bytes
            ABEF = _mm_sha256rnds2_epu32(ABEF, CDGH, msg);
            DEBUG_ABCDEFGH(ABEF, CDGH);

            t_0 = _mm_alignr_epi8(_msg3, _msg2, 4); // W[t-7] for RUN 48
            _msg0 = _mm_add_epi32(_msg0, t_0); // W[t-16] + sigma0 + W[t-7] 
            _msg1 = _mm_sha256msg1_epu32(_msg1, _msg2); // SIGMA0 for RUN 52

            // round 48 - 51
            _msg0 = _mm_sha256msg2_epu32(_msg0, _msg3);
            msg = _mm_add_epi32(_msg0, _mm_set_epi32(0x34b0bcb5, 0x2748774c, 0x1e376c08, 0x19a4c116));
            CDGH = _mm_sha256rnds2_epu32(CDGH, ABEF, msg);
            DEBUG_ABCDEFGH(CDGH, ABEF);
            msg = _mm_shuffle_epi32(msg, 0b00001110); // flip 64 bytes
            ABEF = _mm_sha256rnds2_epu32(ABEF, CDGH, msg);
            DEBUG_ABCDEFGH(ABEF, CDGH);

            t_0 = _mm_alignr_epi8(_msg0, _msg3, 4); // W[t-7] for RUN 52
            _msg1 = _mm_add_epi32(_msg1, t_0); // W[t-16] + sigma0 + W[t-7] 
            _msg2 = _mm_sha256msg1_epu32(_msg2, _msg3); // SIGMA0 for RUN 56

            // round 52 - 55
            _msg1 = _mm_sha256msg2_epu32(_msg1, _msg0);
            msg = _mm_add_epi32(_msg1, _mm_set_epi32(0x682e6ff3, 0x5b9cca4f, 0x4ed8aa4a, 0x391c0cb3));
            CDGH = _mm_sha256rnds2_epu32(CDGH, ABEF, msg);
            DEBUG_ABCDEFGH(CDGH, ABEF);
            msg = _mm_shuffle_epi32(msg, 0b00001110); // flip 64 bytes
            ABEF = _mm_sha256rnds2_epu32(ABEF, CDGH, msg);
            DEBUG_ABCDEFGH(ABEF, CDGH);

            t_0 = _mm_alignr_epi8(_msg1, _msg0, 4); // W[t-7] for RUN 56
            _msg2 = _mm_add_epi32(_msg2, t_0); // W[t-16] + sigma0 + W[t-7] 
            _msg3 = _mm_sha256msg1_epu32(_msg3, _msg0); // SIGMA0 for RUN 60

            // round 55 - 59
            _msg2 = _mm_sha256msg2_epu32(_msg2, _msg1);
            msg = _mm_add_epi32(_msg2, _mm_set_epi32(0x8cc70208, 0x84c87814, 0x78a5636f, 0x748f82ee));
            CDGH = _mm_sha256rnds2_epu32(CDGH, ABEF, msg);
            DEBUG_ABCDEFGH(CDGH, ABEF);
            msg = _mm_shuffle_epi32(msg, 0b00001110); // flip 64 bytes
            ABEF = _mm_sha256rnds2_epu32(ABEF, CDGH, msg);
            DEBUG_ABCDEFGH(ABEF, CDGH);

            t_0 = _mm_alignr_epi8(_msg2, _msg1, 4); // W[t-7] for RUN 60
            _msg3 = _mm_add_epi32(_msg3, t_0); // W[t-16] + sigma0 + W[t-7] 
            _msg0 = _mm_sha256msg1_epu32(_msg0, _msg1); // SIGMA0 for RUN 64

            // round 60 - 63
            _msg3 = _mm_sha256msg2_epu32(_msg3, _msg2);
            msg = _mm_add_epi32(_msg3, _mm_set_epi32(0xc67178f2, 0xbef9a3f7, 0xa4506ceb, 0x90befffa));
            CDGH = _mm_sha256rnds2_epu32(CDGH, ABEF, msg);
            DEBUG_ABCDEFGH(CDGH, ABEF);
            msg = _mm_shuffle_epi32(msg, 0b00001110); // flip 64 bytes
            ABEF = _mm_sha256rnds2_epu32(ABEF, CDGH, msg);
            DEBUG_ABCDEFGH(ABEF, CDGH);

            t_0 = _mm_alignr_epi8(_msg3, _msg2, 4); // W[t-7] for RUN 64
            _msg0 = _mm_add_epi32(_msg0, t_0); // W[t-16] + sigma0 + W[t-7] 

            hash_ABEF = _mm_add_epi32(hash_ABEF, ABEF);
            hash_CDGH = _mm_add_epi32(hash_CDGH, CDGH);
            DEBUG_ABCDEFGH(hash_ABEF, hash_CDGH);
        }

        hash_CDGH = _mm_shuffle_epi32(hash_CDGH, 0b00'01'10'11); // H G D C
        hash_ABEF = _mm_shuffle_epi32(hash_ABEF, 0b10'11'00'01); // B A F E
        __m128i ABCD = _mm_alignr_epi8(hash_CDGH, hash_ABEF, 8);
        __m128i EFGH = _mm_blend_epi32(hash_ABEF, hash_CDGH, 0b0000'1'1'0'0); // H G F E
        ABCD = _mm_shuffle_epi8(ABCD, big_endian_flip_mask);
        EFGH = _mm_shuffle_epi8(EFGH, big_endian_flip_mask);
        
#ifdef DEBUG
        std::cout << std::hex << std::setfill('0') << "ABCD = ";
        word ABCD_words[4];
        _mm_storeu_si128((__m128i_u*) ABCD_words, ABCD);
        for (int i = 0; i < 4; ++i) std::cout << std::setw(8) << ABCD_words[i] << " ";
        std::cout << "\nEFGH = ";
        _mm_storeu_si128((__m128i_u*) ABCD_words, EFGH);
        for (int i = 0; i < 4; ++i) std::cout << std::setw(8) << ABCD_words[i] << " ";
        std::cout << std::dec << std::setfill(' ') << "\n";
#endif

        word final_hash[8];
        _mm_storeu_si128((__m128i_u*) final_hash, ABCD);
        _mm_storeu_si128((__m128i_u*) (final_hash + 4), EFGH);

        delete[] blks;
        return { final_hash };
    }

#endif

}

[[nodiscard]] Hash<256> sha256(const char* input)
{
#ifdef __SHA__
    return _details::instruction_sha256(input);
#elif __AVX2__
    return _details::simd_sha256(input);
#else
    return _details::general_sha256(input);
#endif
}