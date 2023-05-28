#ifndef PROCESS_HPP
#define PROCESS_HPP

#include <fstream>
#include <algorithm>
#include <vector>
#include <string>
#include <atomic>

#include "Hash.hpp"

#include "threadsafe/List.hpp"
#include "threadsafe/Queue.hpp"

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

template<std::size_t N>
void make_hash_list(std::ifstream& hash_file, List<Hash<N>>& data)
{
    std::string s;
    Hash<N> hash;
    while (std::getline(hash_file, s))
    {
        s.erase(std::remove_if(std::begin(s), std::end(s), ::isspace), std::end(s)); // strip spaces
        if (!s.empty())
        {
            hash = str_to_hash<N>(s.c_str());
            data.push_front(hash);
        }
    }
}

using Chunk = std::vector<std::string>;

Chunk create_chunk(std::ifstream& word_file, std::size_t lines);

void hash_crack_sha256(std::ifstream& hash_file, std::ifstream& word_file);

void hash_crack_sha224(std::ifstream& hash_file, std::ifstream& word_file);

void hash_crack_sha512(std::ifstream& hash_file, std::ifstream& word_file);

void hash_crack_sha384(std::ifstream& hash_file, std::ifstream& word_file);

// multithread

void chunk_scheduler(std::ifstream& word_file, Queue<Chunk>& scheduler, std::size_t thread_count, std::atomic<bool>& eoh_flag);

void process_sha256_chunk(List<Hash<256>>& hashes, std::vector<std::string>& word_chunk, std::atomic<bool>& eoh_flag, std::atomic<unsigned int>& hashes_caught);

void process_sha224_chunk(List<Hash<224>>& hashes, std::vector<std::string>& word_chunk, std::atomic<bool>& eoh_flag, std::atomic<unsigned int>& hashes_caught);

void process_sha512_chunk(List<Hash<512>>& hashes, std::vector<std::string>& word_chunk, std::atomic<bool>& eoh_flag, std::atomic<unsigned int>& hashes_caught);

void process_sha384_chunk(List<Hash<384>>& hashes, std::vector<std::string>& word_chunk, std::atomic<bool>& eoh_flag, std::atomic<unsigned int>& hashes_caught);

void scheduling_worker_thread(std::ifstream& word_file, Queue<Chunk>& scheduler, std::size_t thread_count, std::atomic<bool>& eos_flag, std::atomic<bool>& eoh_flag);

void cracking_sha256_worker_thread(List<Hash<256>>& hashes, Queue<Chunk>& scheduler, std::atomic<bool>& eos, std::atomic<bool>& eoh, std::atomic<unsigned int>& hashes_caught);

void cracking_sha224_worker_thread(List<Hash<224>>& hashes, Queue<Chunk>& scheduler, std::atomic<bool>& eos, std::atomic<bool>& eoh, std::atomic<unsigned int>& hashes_caught);

void cracking_sha512_worker_thread(List<Hash<512>>& hashes, Queue<Chunk>& scheduler, std::atomic<bool>& eos, std::atomic<bool>& eoh, std::atomic<unsigned int>& hashes_caught);

void cracking_sha384_worker_thread(List<Hash<384>>& hashes, Queue<Chunk>& scheduler, std::atomic<bool>& eos, std::atomic<bool>& eoh, std::atomic<unsigned int>& hashes_caught);

void multithread_sha256_cracker(std::ifstream& word_file, std::ifstream& hash_file, std::size_t thread_count);

void multithread_sha224_cracker(std::ifstream& word_file, std::ifstream& hash_file, std::size_t thread_count);

void multithread_sha512_cracker(std::ifstream& word_file, std::ifstream& hash_file, std::size_t thread_count);

void multithread_sha384_cracker(std::ifstream& word_file, std::ifstream& hash_file, std::size_t thread_count);

#endif