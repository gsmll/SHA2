#include "shashaker/Process.hpp"

#include <thread>
#include <functional>
#include <cassert>

#include "hash/SHA.hpp"

Chunk create_chunk(std::ifstream& word_file, std::size_t lines)
{
    std::vector<std::string> data;
    data.reserve(lines);

    std::size_t i = 0;
    std::string s;
    while (i < lines && std::getline(word_file, s))
    {
        if (!s.empty())
        {
            data.push_back(s);
            ++i;
        }
    }
    return data;
}

void hash_crack_sha256(std::ifstream& hash_file, std::ifstream& word_file)
{
    constexpr std::size_t ChunkSize = 10000;

    std::vector<Hash<256>> hashes = read_hash_file<256>(hash_file);
    hash_file.close();

    Chunk words = create_chunk(word_file, ChunkSize);
    Hash<256> word_hash;

    while (words.size() > 0)
    {
        for (std::string word : words)
        {
            word_hash = sha256(word.c_str());
            auto res = std::find(std::begin(hashes), std::end(hashes), word_hash);
            if (res != std::end(hashes))
            {
                std::cout << "Hash - \u001b[34;1m" << word_hash << "\u001b[0m\n";
                std::cout << "\tPassword - \u001b[32;1m" << word << "\u001b[0m\n";
                hashes.erase(res);
            }
        }
        words = create_chunk(word_file, ChunkSize);
    }

    std::cout << "\nFailed to crack the following hashes:\n\u001b[31;1m";
    for (auto&& hash : hashes)
    {
        std::cout << hash << "\n";
    }
    std::cout << "\u001b[0m\n";
}

void hash_crack_sha224(std::ifstream& hash_file, std::ifstream& word_file)
{
    constexpr std::size_t ChunkSize = 10000;

    std::vector<Hash<224>> hashes = read_hash_file<224>(hash_file);
    hash_file.close();

    Chunk words = create_chunk(word_file, ChunkSize);
    Hash<224> word_hash;

    while (words.size() > 0)
    {
        for (std::string word : words)
        {
            word_hash = sha224(word.c_str());
            auto res = std::find(std::begin(hashes), std::end(hashes), word_hash);
            if (res != std::end(hashes))
            {
                std::cout << "Hash - \u001b[34;1m" << word_hash << "\u001b[0m\n";
                std::cout << "\tPassword - \u001b[32;1m" << word << "\u001b[0m\n";
                hashes.erase(res);
            }
        }
        words = create_chunk(word_file, ChunkSize);
    }

    std::cout << "\nFailed to crack the following hashes:\n\u001b[31;1m";
    for (auto&& hash : hashes)
    {
        std::cout << hash << "\n";
    }
    std::cout << "\u001b[0m\n";
}

void hash_crack_sha512(std::ifstream& hash_file, std::ifstream& word_file)
{
    constexpr std::size_t ChunkSize = 10000;

    std::vector<Hash<512>> hashes = read_hash_file<512>(hash_file);
    hash_file.close();

    Chunk words = create_chunk(word_file, ChunkSize);
    Hash<512> word_hash;

    while (words.size() > 0)
    {
        for (std::string word : words)
        {
            word_hash = sha512(word.c_str());
            auto res = std::find(std::begin(hashes), std::end(hashes), word_hash);
            if (res != std::end(hashes))
            {
                std::cout << "Hash - \u001b[34;1m" << word_hash << "\u001b[0m\n";
                std::cout << "\tPassword - \u001b[32;1m" << word << "\u001b[0m\n";
                hashes.erase(res);
            }
        }
        words = create_chunk(word_file, ChunkSize);
    }

    std::cout << "\nFailed to crack the following hashes:\n\u001b[31;1m";
    for (auto&& hash : hashes)
    {
        std::cout << hash << "\n";
    }
    std::cout << "\u001b[0m\n";
}

void hash_crack_sha384(std::ifstream& hash_file, std::ifstream& word_file)
{
    constexpr std::size_t ChunkSize = 10000;

    std::vector<Hash<384>> hashes = read_hash_file<384>(hash_file);
    hash_file.close();

    Chunk words = create_chunk(word_file, 100);
    Hash<384> word_hash;

    while (words.size() > 0)
    {
        for (std::string word : words)
        {
            word_hash = sha384(word.c_str());
            auto res = std::find(std::begin(hashes), std::end(hashes), word_hash);
            if (res != std::end(hashes))
            {
                std::cout << "Hash - \u001b[34;1m" << word_hash << "\u001b[0m\n";
                std::cout << "\tPassword - \u001b[32;1m" << word << "\u001b[0m\n";
                hashes.erase(res);
            }
        }
        words = create_chunk(word_file, ChunkSize);
    }

    std::cout << "\nFailed to crack the following hashes:\n\u001b[31;1m";
    for (auto&& hash : hashes)
    {
        std::cout << hash << "\n";
    }
    std::cout << "\u001b[0m\n";
}


void chunk_scheduler(std::ifstream& word_file, Queue<Chunk>& scheduler, std::size_t thread_count, std::atomic<bool>& eoh_flag)
{
    constexpr std::size_t CHUNKS_PENDING_PER_THREAD = 2;
    constexpr std::size_t WORDS_PER_CHUNK = 100;
    std::size_t max_pending_chunks = CHUNKS_PENDING_PER_THREAD * thread_count;

    while (!word_file.eof() && !eoh_flag)
    {
        std::cout << scheduler.size() << std::endl;
        if (eoh_flag) scheduler.terminate();

        if (scheduler.size() < max_pending_chunks)
        {
            Chunk new_chunk{ create_chunk(word_file, WORDS_PER_CHUNK) };
            std::cout << new_chunk[0] << "\n";
            scheduler.push(std::move(new_chunk));
        }
        else
        {
            std::this_thread::yield();
        }
    }
    std::cout << eoh_flag << "\n";
    if (eoh_flag) scheduler.terminate();
    word_file.close();
}

void process_sha256_chunk(List<Hash<256>>& hashes, std::vector<std::string>& word_chunk, std::atomic<bool>& eoh_flag, std::atomic<unsigned int>& hashes_caught)
{
    std::stringstream ss;
    bool expected = false;
    Hash<256> hash;
    for (std::string word : word_chunk)
    {
        if (eoh_flag) return;
        hash = sha256(word.c_str());
        if (hashes.find_first_if([hash](const Hash<256>& h){ return hash == h; }))
        {
            ss << "Hash - \u001b[34;1m" << hash << "\u001b[0m\n";
            ss << "\tPassword - \u001b[32;1m" << word << "\u001b[0m\n"; 
            ++hashes_caught;

            std::cout << ss.str();
            ss.str(std::string{});

            if (hashes.size() == hashes_caught)
            {
                while (!eoh_flag.compare_exchange_weak(expected, true) && !expected); // set eoh_flag to true
                return;
            }
        }
    }
}

void process_sha224_chunk(List<Hash<224>>& hashes, std::vector<std::string>& word_chunk, std::atomic<bool>& eoh_flag, std::atomic<unsigned int>& hashes_caught)
{
    std::stringstream ss;
    bool expected = false;
    Hash<224> hash;
    for (std::string word : word_chunk)
    {
        if (eoh_flag) return;
        hash = sha224(word.c_str());
        if (hashes.remove_first_if([hash](const Hash<224>& h){ return hash == h; }))
        {
            ss << "Hash - \u001b[34;1m" << hash << "\u001b[0m\n";
            ss << "\tPassword - \u001b[32;1m" << word << "\u001b[0m\n"; 
            ++hashes_caught;

            std::cout << ss.str();
            ss.str(std::string{});

            if (hashes.size() == hashes_caught)
            {
                while (!eoh_flag.compare_exchange_weak(expected, true) && !expected); // set eoh_flag to true
                return;
            }
        }
    }
}

void process_sha512_chunk(List<Hash<512>>& hashes, std::vector<std::string>& word_chunk, std::atomic<bool>& eoh_flag, std::atomic<unsigned int>& hashes_caught)
{
    std::stringstream ss;
    bool expected = false;
    Hash<512> hash;
    for (std::string word : word_chunk)
    {
        if (eoh_flag) return;
        hash = sha512(word.c_str());
        if (hashes.remove_first_if([hash](const Hash<512>& h){ return hash == h; }))
        {
            ss << "Hash - \u001b[34;1m" << hash << "\u001b[0m\n";
            ss << "\tPassword - \u001b[32;1m" << word << "\u001b[0m\n"; 
            ++hashes_caught;

            std::cout << ss.str();
            ss.str(std::string{});

            if (hashes.size() == hashes_caught)
            {
                while (!eoh_flag.compare_exchange_weak(expected, true) && !expected); // set eoh_flag to true
                return;
            }
        }
    }
}

void process_sha384_chunk(List<Hash<384>>& hashes, std::vector<std::string>& word_chunk, std::atomic<bool>& eoh_flag, std::atomic<unsigned int>& hashes_caught)
{
    std::stringstream ss;
    bool expected = false;
    Hash<384> hash;
    for (std::string word : word_chunk)
    {
        if (eoh_flag) return;
        hash = sha384(word.c_str());
        if (hashes.remove_first_if([hash](const Hash<384>& h){ return hash == h; }))
        {
            ss << "Hash - \u001b[34;1m" << hash << "\u001b[0m\n";
            ss << "\tPassword - \u001b[32;1m" << word << "\u001b[0m\n"; 
            ++hashes_caught;

            std::cout << ss.str();
            ss.str(std::string{});

            if (hashes.size() == hashes_caught)
            {
                while (!eoh_flag.compare_exchange_weak(expected, true) && !expected); // set eoh_flag to true
                return;
            }
        }
    }
}

void scheduling_worker_thread(std::ifstream& word_file, Queue<Chunk>& scheduler, std::size_t thread_count, std::atomic<bool>& eos_flag, std::atomic<bool>& eoh_flag)
{
    bool expected = true;
    while (!eos_flag.compare_exchange_weak(expected, false) && expected); // set eos_flag to false

    constexpr std::size_t CHUNKS_PENDING_PER_THREAD = 2;
    constexpr std::size_t WORDS_PER_CHUNK = 50000;
    std::size_t max_pending_chunks = CHUNKS_PENDING_PER_THREAD * thread_count;

    while (!word_file.eof() && !eoh_flag)
    {
        if (scheduler.size() < max_pending_chunks)
        {
            Chunk new_chunk{ create_chunk(word_file, WORDS_PER_CHUNK) };
            scheduler.push(std::move(new_chunk));
        }
        else
        {
            std::this_thread::yield();
        }
    }

    if (eoh_flag) scheduler.terminate();
    if (word_file.eof())
    {
        expected = false;
        while (!eos_flag.compare_exchange_weak(expected, true) && !expected); // set eos_flag to true
    }
    word_file.close();
}

void cracking_sha256_worker_thread(List<Hash<256>>& hashes, Queue<Chunk>& scheduler, std::atomic<bool>& eos, std::atomic<bool>& eoh, std::atomic<unsigned int>& hashes_caught)
{
    while (!eoh || !(eos && scheduler.empty()))
    {
        Chunk chunk;
        scheduler.wait_and_pop(chunk);  
        if (eoh) return;
        process_sha256_chunk(hashes, chunk, eoh, hashes_caught);
        // std::cout << "Done with one chunk\n";
    }
}

void cracking_sha224_worker_thread(List<Hash<224>>& hashes, Queue<Chunk>& scheduler, std::atomic<bool>& eos, std::atomic<bool>& eoh, std::atomic<unsigned int>& hashes_caught)
{
    while (!eoh || !(eos && scheduler.empty()))
    {
        Chunk chunk;
        scheduler.wait_and_pop(chunk); 
        if (eoh) return;       
        process_sha224_chunk(hashes, chunk, eoh, hashes_caught);
    }
}

void cracking_sha512_worker_thread(List<Hash<512>>& hashes, Queue<Chunk>& scheduler, std::atomic<bool>& eos, std::atomic<bool>& eoh, std::atomic<unsigned int>& hashes_caught)
{
    while (!eoh || !(eos && scheduler.empty()))
    {
        Chunk chunk;
        scheduler.wait_and_pop(chunk);  
        if (eoh) return;      
        process_sha512_chunk(hashes, chunk, eoh, hashes_caught);
    }
}

void cracking_sha384_worker_thread(List<Hash<384>>& hashes, Queue<Chunk>& scheduler, std::atomic<bool>& eos, std::atomic<bool>& eoh, std::atomic<unsigned int>& hashes_caught)
{
    while (!eoh || !(eos && scheduler.empty()))
    {
        Chunk chunk;
        scheduler.wait_and_pop(chunk); 
        if (eoh) return;
        process_sha384_chunk(hashes, chunk, eoh, hashes_caught);
    }
}

void multithread_sha256_cracker(std::ifstream& word_file, std::ifstream& hash_file, std::size_t thread_count)
{
    assert(thread_count > 1 && "More than one thread required.");
    std::atomic<bool> eos = false;
    std::atomic<bool> eoh = false;
    std::atomic<unsigned int> hashes_caught = 0;

    List<Hash<256>> hashes;
    Queue<Chunk> scheduler{};
    make_hash_list(hash_file, hashes);

    std::vector<std::thread> threads;
    threads.reserve(thread_count - 1);
    for (std::size_t i = 1; i < thread_count; ++i)
    {
        threads.emplace_back(cracking_sha256_worker_thread, std::ref(hashes), std::ref(scheduler), std::ref(eos), std::ref(eoh), std::ref(hashes_caught));
    }

    scheduling_worker_thread(word_file, scheduler, thread_count - 1, eos, eoh);

    for (auto&& thread : threads)
    {
        thread.join();
    }
}

void multithread_sha224_cracker(std::ifstream& word_file, std::ifstream& hash_file, std::size_t thread_count)
{
    assert(thread_count > 1 && "More than one thread required.");
    std::atomic<bool> eos = false;
    std::atomic<bool> eoh = false;
    std::atomic<unsigned int> hashes_caught = 0;

    List<Hash<224>> hashes;
    Queue<Chunk> scheduler{};
    make_hash_list(hash_file, hashes);

    std::vector<std::thread> threads;
    threads.reserve(thread_count - 1);
    for (std::size_t i = 1; i < thread_count; ++i)
    {
        threads.emplace_back(cracking_sha224_worker_thread, std::ref(hashes), std::ref(scheduler), std::ref(eos), std::ref(eoh), std::ref(hashes_caught));
    }

    scheduling_worker_thread(word_file, scheduler, thread_count - 1, eos, eoh);

    for (auto&& thread : threads)
    {
        thread.join();
    }
}

void multithread_sha512_cracker(std::ifstream& word_file, std::ifstream& hash_file, std::size_t thread_count)
{
    assert(thread_count > 1 && "More than one thread required.");
    std::atomic<bool> eos = false;
    std::atomic<bool> eoh = false;
    std::atomic<unsigned int> hashes_caught = 0;

    List<Hash<512>> hashes;
    Queue<Chunk> scheduler{};
    make_hash_list(hash_file, hashes);

    std::vector<std::thread> threads;
    threads.reserve(thread_count - 1);
    for (std::size_t i = 1; i < thread_count; ++i)
    {
        threads.emplace_back(cracking_sha512_worker_thread, std::ref(hashes), std::ref(scheduler), std::ref(eos), std::ref(eoh), std::ref(hashes_caught));
    }

    scheduling_worker_thread(word_file, scheduler, thread_count - 1, eos, eoh);

    for (auto&& thread : threads)
    {
        thread.join();
    }
}

void multithread_sha384_cracker(std::ifstream& word_file, std::ifstream& hash_file, std::size_t thread_count)
{
    assert(thread_count > 1 && "More than one thread required.");
    std::atomic<bool> eos = false;
    std::atomic<bool> eoh = false;
    std::atomic<unsigned int> hashes_caught = 0;

    List<Hash<384>> hashes;
    Queue<Chunk> scheduler{};
    make_hash_list(hash_file, hashes);

    std::vector<std::thread> threads;
    threads.reserve(thread_count - 1);
    for (std::size_t i = 1; i < thread_count; ++i)
    {
        threads.emplace_back(cracking_sha384_worker_thread, std::ref(hashes), std::ref(scheduler), std::ref(eos), std::ref(eoh), std::ref(hashes_caught));
    }

    scheduling_worker_thread(word_file, scheduler, thread_count - 1, eos, eoh);

    for (auto&& thread : threads)
    {
        thread.join();
    }
}