#ifndef ARGUMENT_PARSER
#define ARGUMENT_PARSER

#include <algorithm>
#include <regex>
#include <type_traits>

#include <iostream>

class ArgumentParser
{
private:
    int _argc;
    const char** _argv;
public:
    ArgumentParser() = delete;
    ArgumentParser(const ArgumentParser&) = delete;
    ArgumentParser(ArgumentParser&&) = delete;
    ArgumentParser& operator=(const ArgumentParser&) = delete;
    ArgumentParser& operator=(ArgumentParser&&) = delete;

    ArgumentParser(int argc, const char** argv);

    bool option_exists(const char* option_name) const;

    const char* get_option_argument(const char* option_name) const;
};

#endif