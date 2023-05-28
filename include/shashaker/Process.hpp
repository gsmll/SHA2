#ifndef PROCESS_HPP
#define PROCESS_HPP

#include <fstream>
#include <algorithm>
#include <vector>
#include <string>

#include "Hash.hpp"

template<std::size_t N>
std::vector<Hash<N>> read_hash_file(std::ifstream& hash_file)
{
    std::vector<Hash<N>> data;
    std::string s;
    while (std::getline(hash_file, s))
    {
        s.erase(std::remove_if(std::begin(s), std::end(s), ::isspace), std::end(s)); // strip spaces
        if (!s.empty())
        {
            data.push_back(str_to_hash<N>(s.c_str()));
        }
    }
    return data;
}

std::vector<std::string> read_word_file(std::ifstream& word_file, std::size_t lines);

void hash_crack_sha256(std::ifstream& hash_file, std::ifstream& word_file);

void hash_crack_sha224(std::ifstream& hash_file, std::ifstream& word_file);

void hash_crack_sha512(std::ifstream& hash_file, std::ifstream& word_file);

void hash_crack_sha384(std::ifstream& hash_file, std::ifstream& word_file);

#endif