#include <iostream>
#include <fstream>
#include "Hash.hpp"
#include "hash/SHA256.hpp"
#include "hash/SHA224.hpp"
#include "hash/SHA512.hpp"
#include "hash/SHA384.hpp"
using namespace std;

int main( int argc,char** argv){
    if(argc != 3){
        cout << "wrong amt of args, use hashlist | wordlist" << "\n";
        return 0;
    }
    ifstream wordlist;
    wordlist.open(argv[2]);
    if (wordlist.is_open()){   //checking whether the file is open
      string s;
      while(getline(wordlist, s)){ //read data from file object and put it into string.
        if(sha256(s.c_str()) == 0x8d969eef6ecad3c29a3a629280e686cf0c3f5d5a86aff3ca12020c923adc6c92_hash){
            cout << "yay" << "\n";
            return 0;
        } //print the data of the string
      }
      wordlist.close(); //close the file object.
   }
}