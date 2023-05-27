#include <iostream>
#include <fstream>
#include "Hash.hpp"
#include "hash/SHA256.hpp"
#include "hash/SHA224.hpp"
#include "hash/SHA512.hpp"
#include "hash/SHA384.hpp"
using namespace std;

int main( int argc,char** argv){
    if(argc != 4){
        cout << "wrong amt of args, use hashlist | wordlist | hash type" << "\n";

        return 0;
    }
 
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
}
