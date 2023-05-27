#include "hash/SHA224.hpp"

namespace _details 
{

    [[nodiscard]] Hash<224> general_sha224(const char* input)
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

        word hash_data[8]{ 0xc1059ed8, 0x367cd507, 0x3070dd17, 0xf70e5939, 0xffc00b31, 0x68581511, 0x64f98fa7, 0xbefa4fa4 };

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
        for (std::size_t i = 0; i < 7; ++i)
        {
            hash_data[i] = to_big_endian(hash_data[i]);
        }

        delete[] blks;
        return { hash_data };
    }

#ifdef __AVX2__

    INLINE static __m128i sig0(__m128i input)
    {
        __m128i a = _mm_srli_epi32(input, 7);
        __m128i b = _mm_srli_epi32(input, 18);
        __m128i c = _mm_srli_epi32(input, 3);

        __m128i d = _mm_slli_epi32(input, 25);
        __m128i e = _mm_slli_epi32(input, 14);

        a = _mm_or_si128(a, d);
        b = _mm_or_si128(b, e);
        c = _mm_xor_si128(a, c);
        return _mm_xor_si128(c, b);
    }

    INLINE static __m128i sig1(__m128i input)
    {
        __m128i a = _mm_srli_epi32(input, 17);
        __m128i b = _mm_srli_epi32(input, 19);
        __m128i c = _mm_srli_epi32(input, 10);

        __m128i d = _mm_slli_epi32(input, 15);
        __m128i e = _mm_slli_epi32(input, 13);

        a = _mm_or_si128(a, d);
        b = _mm_or_si128(b, e);
        c = _mm_xor_si128(a, c);
        return _mm_xor_si128(c, b);
    }

    [[nodiscard]] Hash<224> simd_sha224(const char* input)
    {
        using word = std::uint32_t;

        constexpr std::size_t bits_per_block = 512;
        constexpr std::size_t bytes_per_block = bits_per_block / 8;
        constexpr std::size_t rounds_per_chunk = 64;
        constexpr word round_values[64]{ 
            0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
            0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174
        };

        word hash_data[8]{ 0xc1059ed8, 0x367cd507, 0x3070dd17, 0xf70e5939, 0xffc00b31, 0x68581511, 0x64f98fa7, 0xbefa4fa4 };

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

            // set rotatable temp hashes
            // 0 1 2 3 4 5 6 7
            // a b c d e f g h

            word s1, s2, ch, maj;
            word hash_buffer[8 + rounds_per_chunk];
            word* temp_hash = hash_buffer + rounds_per_chunk;
            std::memcpy(temp_hash, hash_data, 8 * sizeof(word));

            // round 0 - 15
            for (std::size_t i = 0; i < 16; ++i, --temp_hash)
            {
                s1 = rotr(temp_hash[4], 6) ^ rotr(temp_hash[4], 11) ^ rotr(temp_hash[4], 25);
                ch = (temp_hash[4] & temp_hash[5]) ^ (~temp_hash[4] & temp_hash[6]);
                temp0_ = temp_hash[7] + s1 + ch + round_values[i] + msg_schedule_arr[i];
                s2 = rotr(temp_hash[0], 2) ^ rotr(temp_hash[0], 13) ^ rotr(temp_hash[0], 22);
                maj = (temp_hash[0] & temp_hash[1]) ^ (temp_hash[0] & temp_hash[2]) ^ (temp_hash[1] & temp_hash[2]);
                temp1_ = s2 + maj;

                temp_hash[-1] = temp0_ + temp1_;
                temp_hash[3] += temp0_;
            }

            __m128i MSG0, MSG1, MSG2, MSG3;
            __m128i W7, W15, W3, W11;
            __m128i M0;
            __m128i S0, S1;
            __m128i ZERO = _mm_setzero_si128();
            __m128i SUM;

            // round 16 - 19
            MSG0 = _mm_loadu_si128((const __m128i_u*) msg_schedule_arr);            // W[t - 16]
            MSG1 = _mm_loadu_si128((const __m128i_u*) (msg_schedule_arr + 4));      // W[t - 12]
            MSG2 = _mm_loadu_si128((const __m128i_u*) (msg_schedule_arr + 8));      // W[t - 8]
            MSG3 = _mm_loadu_si128((const __m128i_u*) (msg_schedule_arr + 12));     // W[t - 4]

            word sums[4];

#define DIGEST()                                                                                            \
_mm_storeu_si128((__m128i_u*) sums, SUM);                                                                   \
for (std::size_t i = 0; i < 4; ++i, --temp_hash)                                                            \
{                                                                                                           \
    s1 = rotr(temp_hash[4], 6) ^ rotr(temp_hash[4], 11) ^ rotr(temp_hash[4], 25);                           \
    ch = (temp_hash[4] & temp_hash[5]) ^ (~temp_hash[4] & temp_hash[6]);                                    \
    temp0_ = temp_hash[7] + s1 + ch + sums[i];                                                              \
    s2 = rotr(temp_hash[0], 2) ^ rotr(temp_hash[0], 13) ^ rotr(temp_hash[0], 22);                           \
    maj = (temp_hash[0] & temp_hash[1]) ^ (temp_hash[0] & temp_hash[2]) ^ (temp_hash[1] & temp_hash[2]);    \
    temp1_ = s2 + maj;                                                                                      \
                                                                                                            \
    temp_hash[-1] = temp0_ + temp1_;                                                                        \
    temp_hash[3] += temp0_;                                                                                 \
}

            M0 = _mm_alignr_epi8(ZERO, MSG3, 8);    // W[t - 2]
            W15 = _mm_alignr_epi8(MSG1, MSG0, 4);   // W[t - 15]
            W7 = _mm_alignr_epi8(MSG3, MSG2, 4);    // W[t - 7]
            S0 = sig0(W15);
            S1 = sig1(M0);
            S0 = _mm_add_epi32(S0, MSG0);
            S1 = _mm_add_epi32(S1, W7);
            MSG0 = _mm_add_epi32(S0, S1);
            S1 = sig1(MSG0);
            S1 = _mm_unpacklo_epi64(ZERO, S1);
            MSG0 = _mm_add_epi32(MSG0, S1); // W16 - W19

            SUM = _mm_add_epi32(MSG0, _mm_set_epi32(0x240ca1cc, 0x0fc19dc6, 0xefbe4786, 0xe49b69c1));
            DIGEST();

            // round 20 - 23
            M0 = _mm_alignr_epi8(ZERO, MSG0, 8);    // W[t - 2]
            W11 = _mm_alignr_epi8(MSG2, MSG1, 4);   // W[t - 15]
            W3 = _mm_alignr_epi8(MSG0, MSG3, 4);    // W[t - 7]
            S0 = sig0(W11);
            S1 = sig1(M0);
            S0 = _mm_add_epi32(S0, MSG1);
            S1 = _mm_add_epi32(S1, W3);
            MSG1 = _mm_add_epi32(S0, S1);
            S1 = sig1(MSG1);
            S1 = _mm_unpacklo_epi64(ZERO, S1);
            MSG1 = _mm_add_epi32(MSG1, S1);

            SUM = _mm_add_epi32(MSG1, _mm_set_epi32(0x76f988da, 0x5cb0a9dc, 0x4a7484aa, 0x2de92c6f));
            DIGEST();

            // round 24 - 27
            M0 = _mm_alignr_epi8(ZERO, MSG1, 8);    // W[t - 2]
            W15 = _mm_alignr_epi8(MSG1, MSG0, 4);   // W[t - 7]
            S0 = sig0(W7);
            S1 = sig1(M0);
            S0 = _mm_add_epi32(S0, MSG2);
            S1 = _mm_add_epi32(S1, W15);
            MSG2 = _mm_add_epi32(S0, S1);
            S1 = sig1(MSG2);
            S1 = _mm_unpacklo_epi64(ZERO, S1);
            MSG2 = _mm_add_epi32(MSG2, S1);

            SUM = _mm_add_epi32(MSG2, _mm_set_epi32(0xbf597fc7, 0xb00327c8, 0xa831c66d, 0x983e5152));
            DIGEST();

            // round 28 - 31
            M0 = _mm_alignr_epi8(ZERO, MSG2, 8);    // W[t - 2]
            W11 = _mm_alignr_epi8(MSG2, MSG1, 4);   // W[t - 7]
            S0 = sig0(W3);
            S1 = sig1(M0);
            S0 = _mm_add_epi32(S0, MSG3);
            S1 = _mm_add_epi32(S1, W11);
            MSG3 = _mm_add_epi32(S0, S1);
            S1 = sig1(MSG3);
            S1 = _mm_unpacklo_epi64(ZERO, S1);
            MSG3 = _mm_add_epi32(MSG3, S1);

            SUM = _mm_add_epi32(MSG3, _mm_set_epi32(0x14292967, 0x06ca6351, 0xd5a79147, 0xc6e00bf3));
            DIGEST();

            // round 32 - 35
            M0 = _mm_alignr_epi8(ZERO, MSG3, 8);    // W[t - 2]
            W7 = _mm_alignr_epi8(MSG3, MSG2, 4);    // W[t - 7]
            S0 = sig0(W15);
            S1 = sig1(M0);
            S0 = _mm_add_epi32(S0, MSG0);
            S1 = _mm_add_epi32(S1, W7);
            MSG0 = _mm_add_epi32(S0, S1);
            S1 = sig1(MSG0);
            S1 = _mm_unpacklo_epi64(ZERO, S1);
            MSG0 = _mm_add_epi32(MSG0, S1);

            SUM = _mm_add_epi32(MSG0, _mm_set_epi32(0x53380d13, 0x4d2c6dfc, 0x2e1b2138, 0x27b70a85));
            DIGEST();

            // round 36 - 39
            M0 = _mm_alignr_epi8(ZERO, MSG0, 8);    // W[t - 2]
            W3 = _mm_alignr_epi8(MSG0, MSG3, 4);    // W[t - 7]
            S0 = sig0(W11);
            S1 = sig1(M0);
            S0 = _mm_add_epi32(S0, MSG1);
            S1 = _mm_add_epi32(S1, W3);
            MSG1 = _mm_add_epi32(S0, S1);
            S1 = sig1(MSG1);
            S1 = _mm_unpacklo_epi64(ZERO, S1);
            MSG1 = _mm_add_epi32(MSG1, S1);

            SUM = _mm_add_epi32(MSG1, _mm_set_epi32(0x92722c85, 0x81c2c92e, 0x766a0abb, 0x650a7354));
            DIGEST();

            // round 40 - 43
            M0 = _mm_alignr_epi8(ZERO, MSG1, 8);    // W[t - 2]
            W15 = _mm_alignr_epi8(MSG1, MSG0, 4);   // W[t - 7]
            S0 = sig0(W7);
            S1 = sig1(M0);
            S0 = _mm_add_epi32(S0, MSG2);
            S1 = _mm_add_epi32(S1, W15);
            MSG2 = _mm_add_epi32(S0, S1);
            S1 = sig1(MSG2);
            S1 = _mm_unpacklo_epi64(ZERO, S1);
            MSG2 = _mm_add_epi32(MSG2, S1);

            SUM = _mm_add_epi32(MSG2, _mm_set_epi32(0xc76c51a3, 0xc24b8b70, 0xa81a664b, 0xa2bfe8a1));
            DIGEST();

            // round 44 - 47
            M0 = _mm_alignr_epi8(ZERO, MSG2, 8);    // W[t - 2]
            W11 = _mm_alignr_epi8(MSG2, MSG1, 4);   // W[t - 7]
            S0 = sig0(W3);
            S1 = sig1(M0);
            S0 = _mm_add_epi32(S0, MSG3);
            S1 = _mm_add_epi32(S1, W11);
            MSG3 = _mm_add_epi32(S0, S1);
            S1 = sig1(MSG3);
            S1 = _mm_unpacklo_epi64(ZERO, S1);
            MSG3 = _mm_add_epi32(MSG3, S1);

            SUM = _mm_add_epi32(MSG3, _mm_set_epi32(0x106aa070, 0xf40e3585, 0xd6990624, 0xd192e819));
            DIGEST();

            // round 48 - 51
            M0 = _mm_alignr_epi8(ZERO, MSG3, 8);    // W[t - 2]
            W7 = _mm_alignr_epi8(MSG3, MSG2, 4);    // W[t - 7]
            S0 = sig0(W15);
            S1 = sig1(M0);
            S0 = _mm_add_epi32(S0, MSG0);
            S1 = _mm_add_epi32(S1, W7);
            MSG0 = _mm_add_epi32(S0, S1);
            S1 = sig1(MSG0);
            S1 = _mm_unpacklo_epi64(ZERO, S1);
            MSG0 = _mm_add_epi32(MSG0, S1);

            SUM = _mm_add_epi32(MSG0, _mm_set_epi32(0x34b0bcb5, 0x2748774c, 0x1e376c08, 0x19a4c116));
            DIGEST();

            // round 52 - 55
            M0 = _mm_alignr_epi8(ZERO, MSG0, 8);    // W[t - 2]
            W3 = _mm_alignr_epi8(MSG0, MSG3, 4);    // W[t - 7]
            S0 = sig0(W11);
            S1 = sig1(M0);
            S0 = _mm_add_epi32(S0, MSG1);
            S1 = _mm_add_epi32(S1, W3);
            MSG1 = _mm_add_epi32(S0, S1);
            S1 = sig1(MSG1);
            S1 = _mm_unpacklo_epi64(ZERO, S1);
            MSG1 = _mm_add_epi32(MSG1, S1);

            SUM = _mm_add_epi32(MSG1, _mm_set_epi32(0x682e6ff3, 0x5b9cca4f, 0x4ed8aa4a, 0x391c0cb3));
            DIGEST();

            // round 56 - 59
            M0 = _mm_alignr_epi8(ZERO, MSG1, 8);    // W[t - 2]
            W15 = _mm_alignr_epi8(MSG1, MSG0, 4);   // W[t - 7]
            S0 = sig0(W7);
            S1 = sig1(M0);
            S0 = _mm_add_epi32(S0, MSG2);
            S1 = _mm_add_epi32(S1, W15);
            MSG2 = _mm_add_epi32(S0, S1);
            S1 = sig1(MSG2);
            S1 = _mm_unpacklo_epi64(ZERO, S1);
            MSG2 = _mm_add_epi32(MSG2, S1);

            SUM = _mm_add_epi32(MSG2, _mm_set_epi32(0x8cc70208, 0x84c87814, 0x78a5636f, 0x748f82ee));
            DIGEST();

            // round 60 - 63
            M0 = _mm_alignr_epi8(ZERO, MSG2, 8);    // W[t - 2]
            W11 = _mm_alignr_epi8(MSG2, MSG1, 4);   // W[t - 7]
            S0 = sig0(W3);
            S1 = sig1(M0);
            S0 = _mm_add_epi32(S0, MSG3);
            S1 = _mm_add_epi32(S1, W11);
            MSG3 = _mm_add_epi32(S0, S1);
            S1 = sig1(MSG3);
            S1 = _mm_unpacklo_epi64(ZERO, S1);
            MSG3 = _mm_add_epi32(MSG3, S1);

            SUM = _mm_add_epi32(MSG3, _mm_set_epi32(0xc67178f2, 0xbef9a3f7, 0xa4506ceb, 0x90befffa));
            DIGEST();

            for (std::size_t i = 0; i < 8; ++i)
            {
                hash_data[i] += temp_hash[i];
            }
        }

        // flip back if necessary
        for (std::size_t i = 0; i < 7; ++i)
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

    [[nodiscard]] Hash<224> instruction_sha224(const char* input)
    {
        using word = std::uint32_t;

        constexpr std::size_t bits_per_block = 512;
        constexpr std::size_t bytes_per_block = bits_per_block / 8;

        std::size_t blk_count;
        std::uint8_t* blks = preprocess_sha256(input, &blk_count);

        __m128i hash_CDGH = _mm_set_epi32(0x3070dd17, 0xf70e5939, 0x64f98fa7, 0xbefa4fa4);
        __m128i hash_ABEF = _mm_set_epi32(0xc1059ed8, 0x367cd507, 0xffc00b31, 0x68581511); 

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

[[nodiscard]] Hash<224> sha224(const char* input)
{
/* #ifdef __SHA__
    return _details::instruction_sha224(input); */
#ifdef __AVX2__
    return _details::simd_sha224(input);
#else
    return _details::general_sha224(input);
#endif
}