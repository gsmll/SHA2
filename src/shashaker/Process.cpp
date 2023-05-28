#include "shashaker/Process.hpp"

#include "hash/SHA.hpp"

std::vector<std::string> read_word_file(std::ifstream& word_file, std::size_t lines)
{
    std::vector<std::string> data;
    data.reserve(lines);

    std::size_t i = 0;
    std::string s;
    while (i++ < lines && std::getline(word_file, s))
    {
        s.erase(std::remove_if(std::begin(s), std::end(s), ::isspace), std::end(s)); // strip spaces
        if (!s.empty())
        {
            data.push_back(s);
            --i;
        }
    }
    return data;
}

void hash_crack_sha256(std::ifstream& hash_file, std::ifstream& word_file)
{
    std::vector<Hash<256>> hashes = read_hash_file<256>(hash_file);
    hash_file.close();

    std::vector<std::string> words = read_word_file(word_file, 100);
    Hash<256> word_hash;
    for (std::string word : words)
    {
        word_hash = sha256(word.c_str());
        auto res = std::find(std::begin(hashes), std::end(hashes), word_hash);
        if (res != std::end(hashes))
        {
            std::cout << word_hash << " -- " << word << "\n"; // output
            hashes.erase(res);
        }
    }

    for (auto&& hash : hashes)
    {
        std::cout << "Could not crack " << hash << "\n";
    }
}