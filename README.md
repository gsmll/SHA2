<p align="center">
<font size="5">
  <h3 align="center"><b>Sha Sha Slide</b></h3>
</font>  
  <p align="center">
    Sha-2 Encryption in a Nutshell
    <br> </br>
    <a href="https://tryhackme.com/p/Jeffrey.J">
    <img src="https://i.imgur.com/dODfC6z.gif"  alt="GSmall" width=72 height=72>
    </a>
    <a href="https://tryhackme.com/p/GSmall">
    <img src="https://tryhackme-images.s3.amazonaws.com/user-avatars/c31ea6bfbcc1fa28101976dced850e48.png"  alt="GSmall" width=72 height=72>
    <br> 
    </a>
      <a href="https://tryhackme.com/p/Jeffrey.J">
        Jeffrey Jiang </a>
        <a href="https://tryhackme.com/p/GSmall">
          | Gabriel Small</a>
    </a>
    <br>
    Period 4 - Group 16
  </p>
</p>  

## **Project Description**

This project covers string hashing and encryption using the SHA-2 algorithm.

We implemented the SHA-2 algorithms in C++ in accordance to the standard as described in the [FIPS Publication 180-3](https://csrc.nist.gov/csrc/media/publications/fips/180/3/archive/2008-10-31/documents/fips180-3_final.pdf), along with a hash cracker to test these algorithms against a word list, such as [rockyou](https://github.com/brannondorsey/naive-hashcat/releases/download/data/rockyou.txt). 

## **Installation**

To use this project first clone the repo using:

```
git clone git@github.com:Stuycs-K/final-project-4-jiangj-smallg.git
```

### **Non-Mac Users With WSL2**

If GNU `g++` is not installed, run the following:

```bash
sudo apt install g++
```

***Note**: The SHA256/224 algorithms make use of CPU instruction extensions to be more efficient. The lab machines do not support the fastest version of SHA256/224, but it does support one faster than the standard implementation.*

To determine CPU compatibility, run the following commmand:

```bash
make ext
```

If `AVX2` is not outputted, you are left with the default implementation. The lab machines will likely run the algorithms faster. 

If `SHA` is outputted, you will have the fastest implementation of SHA256/224 on that device. 

Run the following command to test if the algorithms are working correctly:

```bash
make test
```

If any of the tests fail, use the lab machines instead to continue.

Run one of the following:

```bash
make
make all
```

This will create programs found in the `tool\` directory. There will be the `shaXXX` programs and the `Shaker` program. If there is any compilation errors. Use the lab machines.

### **Mac Users**

***Note**: The SHA-2 algorithms are optimized for x86 architecture. Macbooks and such are not x86. As a result, the program will most likely run faster on lab machines. Therefore, it is highly discouraged to run the programs on Mac. If you wish to continue, you will need the following:*

If you can run `g++` in your terminal, make sure `g++` uses GNU's `g++` and not `clang`. *We use GNU's `g++` compiler specific features.*

To check, do the following:

```bash
g++ --version
```

If `clang` appears, you will have to install GNU `g++` on your preferred method such as `homebrew`. 

```bash
brew install gcc
```

Once installed, do one the following:

1. Go into the makefile and change the `CC` variable to the GNU `g++` program you have just installed. 
2. Change the alias of `g++` in the mac to refer to GNU instead of `clang` by running the following command: 

```bash
# replace g++-13 with the appropriate g++ version
sudo ln -s $(which g++-13) /usr/local/bin/g++
```

Now, follow the instructions in the [Non-Mac User Section](#non-mac-users).

If you get stuck anywhere here, remember it is significantly better (faster and easier) to run this program on an x86 machine. The lab machines are always available to just `ssh` into.

## **Usage**

### `shaXXX`

These programs will hash input strings with their respective SHA-2 algorithm.

```bash
tool/sha256 abc 'hello world'
tool/sha256 < rockyou.txt
head -n 5 rockyou.txt | tool/sha256
```

Running `shaXXX` will continuously ask for input and hash it. To exit, issue a `SIGINT` using `ctrl-c`.

### `Shaker`

This is the hash cracking tool. Basic usage is as follows:

```bash
tool/Shaker --wordlist rockyou.txt --hash hash.txt # defaults to sha256
tool/Shaker -w rockyou.txt -h hash.txt # defaults to sha256

tool/Shaker --wordlist rockyou.txt --format sha512 --hash hash.txt
tool/Shaker -w rockyou.txt -f sha512 -h hash.txt
```

`-w` or `--wordlist` option is required. `-h` or `--hash` is also required. The wordlist option accepts a file containing the wordlist. The hash option accepts a file containing a list of hashes to crack. The `-f` or `--format` option accepts either `sha256|sha224|sha512|sha384`.  

## **Links**

### [Homework](HOMEWORK.md)
### [Presentation](PRESENTATION.md)



