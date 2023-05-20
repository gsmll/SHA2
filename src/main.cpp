#include <iostream>

#include "Hash.hpp"
#include "hash/SHA256.hpp"
#include "hash/SHA224.hpp"

#define TEST(expr, msg)          \
if (expr) std::cout << "PASSED TEST [" << msg << "]\n"; \
else std::cout << "FAILED TEST [" << msg << "]\n"; 

int main()
{
    TEST(sha256("abc") == 0xba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad_hash, "SHA256 SHORT MESSAGE TEST");
    TEST(sha256("abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq") == 0x248d6a61d20638b8e5c026930c3e6039a33ce45964ff2167f6ecedd419db06c1_hash, "SHA256 MEDIUM MESSAGE TEST");

    char* buffer = new char[1000001];
    buffer[1000000] = '\0';
    for (std::size_t i = 0; i < 1000000; ++i)
    {
        buffer[i] = 'a';
    }
    TEST(sha256(buffer) == 0xcdc76e5c9914fb9281a1c7e284d73e67f1809a48a497200e046d39ccc7112cd0_hash, "SHA256 LONG LONG MESSAGE TEST");
    delete[] buffer;

    TEST(sha224("hello world") == 0x2f05477fc24bb4faefd86517156dafdecec45b8ad3cf2522a563582b_hash, "SHA224 SHORT MESSAGE TEST");
    TEST(sha224("abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq") == 0x75388b16512776cc5dba5da1fd890150b0c6455cb4f58b1952522525_hash, "SHA224 MEDIUM MESSAGE TEST");
}