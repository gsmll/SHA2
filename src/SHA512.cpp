#include "hash/SHA512.hpp"

#include <cstring>

#ifdef DEBUG
#include <iomanip>
#include <bitset>
#endif

namespace _details 
{

    [[nodiscard]] std::uint8_t* preprocess_sha512(const char* input, std::size_t* blk_total)
    {
        constexpr std::size_t BLOCK_BYTE_SIZE = 1024 / 8;

        std::size_t strsize = std::strlen(input);
        *blk_total = (strsize + 16 + 1) / 128 + ( (strsize + 16 + 1) % 128 ? 1 : 0 );
        std::uint8_t* blks = new std::uint8_t[BLOCK_BYTE_SIZE * *blk_total]{ }; // paren important        
        std::memcpy(blks, input, strsize);
        blks[strsize] |= 1 << 7;
        std::uint64_t strbitsize = to_big_endian(8ul * strsize);
        std::memcpy(blks + *blk_total * BLOCK_BYTE_SIZE - sizeof(std::uint64_t), &strbitsize, sizeof(std::uint64_t));

#ifdef DEBUG
        std::cout << "PADDED MESSAGE\n" << std::hex << std::setfill('0');
        for (std::size_t i = 0; i < BLOCK_BYTE_SIZE * *blk_total; ++i)
        {
            std::cout << std::setw(2) << +blks[i];
            if (i % 8 == 7) std::cout << "\n";
        }
        std::cout << "\n" << std::dec << std::setfill(' ');
#endif

        return blks;
    }

   
    [[nodiscard]] Hash<512> general_sha512(const char* input)
    {
                using word = std::uint64_t;

        constexpr std::size_t bits_per_block = 1024;
        constexpr std::size_t bytes_per_block = bits_per_block / 8;
        constexpr std::size_t rounds_per_chunk = 80;
        constexpr word round_values[80]{0x428a2f98d728ae22, 0x7137449123ef65cd, 0xb5c0fbcfec4d3b2f, 0xe9b5dba58189dbbc, 0x3956c25bf348b538, 
            0x59f111f1b605d019, 0x923f82a4af194f9b, 0xab1c5ed5da6d8118, 0xd807aa98a3030242, 0x12835b0145706fbe, 
            0x243185be4ee4b28c, 0x550c7dc3d5ffb4e2, 0x72be5d74f27b896f, 0x80deb1fe3b1696b1, 0x9bdc06a725c71235, 
            0xc19bf174cf692694, 0xe49b69c19ef14ad2, 0xefbe4786384f25e3, 0x0fc19dc68b8cd5b5, 0x240ca1cc77ac9c65, 
            0x2de92c6f592b0275, 0x4a7484aa6ea6e483, 0x5cb0a9dcbd41fbd4, 0x76f988da831153b5, 0x983e5152ee66dfab, 
            0xa831c66d2db43210, 0xb00327c898fb213f, 0xbf597fc7beef0ee4, 0xc6e00bf33da88fc2, 0xd5a79147930aa725, 
            0x06ca6351e003826f, 0x142929670a0e6e70, 0x27b70a8546d22ffc, 0x2e1b21385c26c926, 0x4d2c6dfc5ac42aed, 
            0x53380d139d95b3df, 0x650a73548baf63de, 0x766a0abb3c77b2a8, 0x81c2c92e47edaee6, 0x92722c851482353b, 
            0xa2bfe8a14cf10364, 0xa81a664bbc423001, 0xc24b8b70d0f89791, 0xc76c51a30654be30, 0xd192e819d6ef5218, 
            0xd69906245565a910, 0xf40e35855771202a, 0x106aa07032bbd1b8, 0x19a4c116b8d2d0c8, 0x1e376c085141ab53, 
            0x2748774cdf8eeb99, 0x34b0bcb5e19b48a8, 0x391c0cb3c5c95a63, 0x4ed8aa4ae3418acb, 0x5b9cca4f7763e373, 
            0x682e6ff3d6b2b8a3, 0x748f82ee5defb2fc, 0x78a5636f43172f60, 0x84c87814a1f0ab72, 0x8cc702081a6439ec, 
            0x90befffa23631e28, 0xa4506cebde82bde9, 0xbef9a3f7b2c67915, 0xc67178f2e372532b, 0xca273eceea26619c, 
            0xd186b8c721c0c207, 0xeada7dd6cde0eb1e, 0xf57d4f7fee6ed178, 0x06f067aa72176fba, 0x0a637dc5a2c898a6, 
            0x113f9804bef90dae, 0x1b710b35131c471b, 0x28db77f523047d84, 0x32caab7b40c72493, 0x3c9ebe0a15c9bebc, 
            0x431d67c49c100d4c, 0x4cc5d4becb3e42b6, 0x597f299cfc657e2a, 0x5fcb6fab3ad6faec, 0x6c44198c4a475817};

        word hash_data[8]{ 0x6a09e667f3bcc908, 0xbb67ae8584caa73b, 0x3c6ef372fe94f82b, 0xa54ff53a5f1d36f1, 
           0x510e527fade682d1, 0x9b05688c2b3e6c1f, 0x1f83d9abfb41bd6b, 0x5be0cd19137e2179 };
        std::size_t blk_count;
        std::uint8_t* blks = preprocess_sha512(input, &blk_count);
        for (std::size_t chunk_idx = 0; chunk_idx < blk_count; ++chunk_idx)
        {
            word temp0_, temp1_;

            // generate message schedule array
            word msg_schedule_arr[80];
            for (std::size_t i = 0; i < 16; ++i) 
            {
                std::memcpy(&temp0_, blks + chunk_idx * bytes_per_block + i * sizeof(word), sizeof(word));
               
                msg_schedule_arr[i] = to_big_endian(temp0_);
              
            }
            for (std::size_t i = 16; i < rounds_per_chunk; ++i)
            {
                temp0_ = rotr(msg_schedule_arr[i - 15], 1) ^ 
                         rotr(msg_schedule_arr[i - 15], 8) ^
                         (msg_schedule_arr[i - 15] >> 7);
                temp1_ = rotr(msg_schedule_arr[i - 2], 19) ^
                         rotr(msg_schedule_arr[i - 2], 61) ^
                         (msg_schedule_arr[i - 2] >> 6);
#ifdef DEBUG

                std::cout << "TEMP0 = " << std::bitset<64>{ temp0_ } << "\nTEMP1 = " << std::bitset<64>{ temp1_ } << "\n";

#endif
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
                s1 = rotr(temp_hash[4], 14) ^ rotr(temp_hash[4], 18) ^ rotr(temp_hash[4], 41);
                ch = (temp_hash[4] & temp_hash[5]) ^ (~temp_hash[4] & temp_hash[6]);
                temp0_ = temp_hash[7] + s1 + ch + round_values[i] + msg_schedule_arr[i];
                s2 = rotr(temp_hash[0], 28) ^ rotr(temp_hash[0], 34) ^ rotr(temp_hash[0], 39);
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
}}

[[nodiscard]] Hash<512> sha512(const char* input)
{
    return _details::general_sha512(input);
}
