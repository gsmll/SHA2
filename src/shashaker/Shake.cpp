#include <iostream>
#include <fstream>
#include <regex>

#include "Hash.hpp"
#include "hash/SHA.hpp"
#include "shashaker/ArgumentParser.hpp"

using namespace std;

constexpr int DEFAULT_THREAD_COUNT = 4;

// ./Shaker --format sha512 --wordlist rockyou.txt --hash hash.txt --thread 4
// ./Shaker -f sha256 -w rockyou.txt -t 20 -h hash.txt
int main(int argc, const char** argv)
{
    ArgumentParser option_parser{ argc, argv };

    std::regex filename_regex{ "^[A-Za-z0-9_\\. ]+$" };
    std::regex sha_regex{ "^sha(256|224|512|384)$" };
    std::regex numeric_regex{ "^([0-9]+|max)$" };

    const char* wordlist = nullptr;
    const char* hashlist = nullptr;
    const char* sha_type = nullptr;
    const char* thread_count_str = nullptr;
    int sha_num = 256;
    int thread_count = DEFAULT_THREAD_COUNT;

    if ((wordlist = option_parser.get_option_argument("-w")))
    {
        if (!std::regex_match(wordlist, filename_regex))
        {
            std::cout << "Incorrectly formatted file name for -w option.\n";
            return 1;
        }
    }
    else if ((wordlist = option_parser.get_option_argument("--wordlist")))
    {
        if (!std::regex_match(wordlist, filename_regex))
        {
            std::cout << "Incorrectly formatted file name for --wordlist option.\n";
            return 1;
        }
    }

    if ((hashlist = option_parser.get_option_argument("-h")))
    {
        if (!std::regex_match(hashlist, filename_regex))
        {
            std::cout << "Incorrectly formatted file name for -h option.\n";
            return 1;
        }
    }
    else if ((hashlist = option_parser.get_option_argument("--hash")))
    {
        if (!std::regex_match(hashlist, filename_regex))
        {
            std::cout << "Incorrectly formatted file name for --hash option.\n";
            return 1;
        }
    }

    if ((sha_type = option_parser.get_option_argument("-f")))
    {
        if (!std::regex_match(sha_type, sha_regex))
        {
            std::cout << "Invalid input for hash type. Only accepts [sha224|sha256|sha384|sha512]\n";
            return 1;
        }

        sha_num = std::stoi(std::string{ sha_type + 3 });
    }
    else if ((sha_type = option_parser.get_option_argument("--format")))
    {
        if (!std::regex_match(sha_type, sha_regex))
        {
            std::cout << "Invalid input for hash type. Only accepts [sha224|sha256|sha384|sha512]\n";
            return 1;
        }

        sha_num = std::stoi(std::string{ sha_type + 3 });
    }

    if ((thread_count_str = option_parser.get_option_argument("-t")))
    {
        if (!std::regex_match(thread_count_str, numeric_regex))
        {
            std::cout << "Invalid input for -t option. Requires integral value.\n";
            return 1;
        }

        thread_count = std::stoi(std::string{ thread_count_str });
    }
    else if ((thread_count_str = option_parser.get_option_argument("--thread")))
    {
        if (!std::regex_match(thread_count_str, numeric_regex))
        {
            std::cout << "Invalid input for --thread option. Requires integral value.\n";
            return 1;
        }

        thread_count = std::stoi(std::string{ thread_count_str });
    }

    if (!wordlist)
    {
        std::cout << "No word list is provided. Please provide one with the --wordlist or -w options.\n";
        return 1;
    }

    if (!hashlist)
    {
        std::cout << "No hash is provided. Please provide a file containing the hashes to the --hash or -h options.\n";
        return 1;
    }

    std::cout << "WORDLIST FILE = " << wordlist << "\n";
    std::cout << "HASH FILE = " << hashlist << "\n";
    std::cout << "HASH TYPE = SHA" << sha_num << "\n";
    std::cout << "THREADS = " << thread_count << "\n";

/*
    ifstream wordlist;
    wordlist.open(argv[2]);
    if (wordlist.is_open()){   //checking whether the file is open
      string s;
       switch(stoi(argv[3])){
        case 224: 
          while(getline(wordlist, s)){ //read data from file object and put it into string.
        if(sha224(s.c_str()) == 0xdeedb07a5cb5344f8c542e1aee5671c721727c0190251c1e5786339c_hash){
            cout << s << "\n";
            return 0;
        } //print the data of the string

         }
         break;
         case 384:
         while(getline(wordlist, s)){ //read data from file object and put it into string.
        if(sha384(s.c_str()) == 0x8212ccd48c6db5c081009f31fbfa5bed19e07ad8be14fe8165d1eb6b9c5b86690365005903691bc999edb2c539c87c89_hash){
            cout << s << "\n";
            return 0;
        } //print the data of the string

         }
         break;
         case 512:
         while(getline(wordlist, s)){ //read data from file object and put it into string.
        if(sha512(s.c_str()) == 0xce9ffea3f93d5e0611bbc59e63f8f9786c2dc421a3d3bed5be7f78cfd29dd8a325057fe675b7d4836178b6528ac8e0aecd3380358bb5f409070462513d45b7db_hash){
          
            cout << s << "\n";
            return 0;
        } //print the data of the string

         }
         break;
         default:
          while(getline(wordlist, s)){ //read data from file object and put it into string.
                  if(sha256(s.c_str()) == 0x95e6e383826bbfbaeb8ed0e23b538ae61105fa688807215ea902b569627b8515_hash){
                      cout << s << "\n";
                      return 0;
                  } //print the data of the string
                }
    }
      
      wordlist.close(); //close the file object.
   }
*/
}
