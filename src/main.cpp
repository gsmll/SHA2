#include <iostream>

#include "Hash.hpp"
#include "hash/SHA256.hpp"
#include "hash/SHA224.hpp"

#define TEST_EQ(name, expr, correct)                            \
{                                                               \
    auto __test_eq{ expr };                                     \
    auto __correct_eq{ correct };                               \
    if (__test_eq == __correct_eq)                              \
        std::cout << "PASSED TEST [" << name << "]\n";          \
    else                                                        \
    {                                                           \
        std::cout << "FAILED TEST [" << name << "]\n";          \
        std::cout << "\tExpected:\t" << correct << "\n";        \
        std::cout << "\tCalculated:\t" << __test_eq << "\n";    \
    }                                                           \
}

int main()
{
    TEST_EQ("SHA256 SHORT MESSAGE", sha256("abc"), 
        0xba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad_hash)

/*     TEST_EQ("SHA256 MEDIUM MESSAGE", sha256("abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq"),
        0x248d6a61d20638b8e5c026930c3e6039a33ce45964ff2167f6ecedd419db06c1_hash)
    
    char* buffer = new char[1000001]{};
    for (std::size_t i = 0; i < 1000000; ++i) buffer[i] = 'a';
    TEST_EQ("SHA256 LONG MESSAGE", sha256(buffer),
        0xcdc76e5c9914fb9281a1c7e284d73e67f1809a48a497200e046d39ccc7112cd0_hash);
    delete[] buffer;

    TEST_EQ("SHA224 SHORT MESSAGE", sha224("hello world"),
        0x2f05477fc24bb4faefd86517156dafdecec45b8ad3cf2522a563582b_hash);

    TEST_EQ("SHA224 MEDIUM MESSAGE", sha224("abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq"),
        0x75388b16512776cc5dba5da1fd890150b0c6455cb4f58b1952522525_hash); */

}