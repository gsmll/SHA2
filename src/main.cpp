#include <iostream>

#include "Hash.hpp"
#include "hash/SHA.hpp"

#define TEST_EQ(name, expr, correct)                                                    \
{                                                                                       \
    auto __test_eq{ expr };                                                             \
    auto __correct_eq{ correct };                                                       \
    if (__test_eq == __correct_eq)                                                      \
        std::cout << "\u001b[32;1mPASSED TEST\u001b[0m [" << name << "]\n";             \
    else                                                                                \
    {                                                                                   \
        std::cout << "\u001b[31;1mFAILED TEST\u001b[0m [" << name << "]\n";             \
        std::cout << "\tExpected:\t\u001b[32;1m" << correct << "\u001b[0m\n";           \
        std::cout << "\tCalculated:\t\u001b[31;1m" << __test_eq << "\u001b[0m\n";       \
    }                                                                                   \
}

int main()
{
    char* buffer = new char[1000001]{};
    for (std::size_t i = 0; i < 1000000; ++i) buffer[i] = 'a';

    TEST_EQ("SHA256 EMPTY MESSAGE", sha256(""),
        0xe3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855_hash)
    TEST_EQ("SHA256 SHORT MESSAGE", sha256("abc"), 
        0xba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad_hash)
    TEST_EQ("SHA256 MEDIUM MESSAGE", sha256("abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq"),
        0x248d6a61d20638b8e5c026930c3e6039a33ce45964ff2167f6ecedd419db06c1_hash)
    TEST_EQ("SHA256 LONG MESSAGE", sha256(buffer),
        0xcdc76e5c9914fb9281a1c7e284d73e67f1809a48a497200e046d39ccc7112cd0_hash)


    TEST_EQ("SHA224 EMPTY MESSAGE", sha224(""),
        0xd14a028c2a3a2bc9476102bb288234c415a2b01f828ea62ac5b3e42f_hash)
    TEST_EQ("SHA224 SHORT MESSAGE", sha224("hello world"),
        0x2f05477fc24bb4faefd86517156dafdecec45b8ad3cf2522a563582b_hash)
    TEST_EQ("SHA224 MEDIUM MESSAGE", sha224("abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq"),
        0x75388b16512776cc5dba5da1fd890150b0c6455cb4f58b1952522525_hash)
    TEST_EQ("SHA224 LONG MESSAGE", sha224(buffer),
        0x20794655980c91d8bbb4c1ea97618a4bf03f42581948b2ee4ee7ad67_hash)


    TEST_EQ("SHA512 EMPTY MESSAGE", sha512(""),
        0xcf83e1357eefb8bdf1542850d66d8007d620e4050b5715dc83f4a921d36ce9ce47d0d13c5d85f2b0ff8318d2877eec2f63b931bd47417a81a538327af927da3e_hash)
    TEST_EQ("SHA512 SHORT MESSAGE", sha512("abc"),
        0xddaf35a193617abacc417349ae20413112e6fa4e89a97ea20a9eeee64b55d39a2192992a274fc1a836ba3c23a3feebbd454d4423643ce80e2a9ac94fa54ca49f_hash)
    TEST_EQ("SHA512 MEDIUM MESSAGE", sha512("abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu"),
        0x8e959b75dae313da8cf4f72814fc143f8f7779c6eb9f7fa17299aeadb6889018501d289e4900f7e4331b99dec4b5433ac7d329eeb6dd26545e96e55b874be909_hash)
    TEST_EQ("SHA512 LONG MESSAGE", sha512(buffer),
        0xe718483d0ce769644e2e42c7bc15b4638e1f98b13b2044285632a803afa973ebde0ff244877ea60a4cb0432ce577c31beb009c5c2c49aa2e4eadb217ad8cc09b_hash)

    TEST_EQ("SHA384 EMPTY MESSAGE", sha384(""),
        0x38b060a751ac96384cd9327eb1b1e36a21fdb71114be07434c0cc7bf63f6e1da274edebfe76f65fbd51ad2f14898b95b_hash)
    TEST_EQ("SHA384 SHORT MESSAGE", sha384("abc"),
        0xcb00753f45a35e8bb5a03d699ac65007272c32ab0eded1631a8b605a43ff5bed8086072ba1e7cc2358baeca134c825a7_hash)
    TEST_EQ("SHA384 MEDIUM MESSAGE", sha384("abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu"),
        0x09330c33f71147e83d192fc782cd1b4753111b173b3b05d22fa08086e3b0f712fcc7c71a557e2db966c3e9fa91746039_hash)
    TEST_EQ("SHA384 LONG MESSAGE", sha384(buffer),
        0x9d0e1809716474cb086e834e310a4a1ced149e9c00f248527972cec5704c2a5b07b8b3dc38ecc4ebae97ddd87f3d8985_hash)

    delete[] buffer;

}