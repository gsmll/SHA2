#include "shashaker/ArgumentParser.hpp"
#include <cstring>
#include <iostream>

ArgumentParser::ArgumentParser(int argc, const char** argv) : _argc{ argc }, _argv{ argv } {}

bool ArgumentParser::option_exists(const char* option_name) const
{
    return std::find_if(_argv, _argv + _argc, 
        [option_name](const char* arg){ return std::strcmp(arg, option_name) == 0; }
    ) != _argv + _argc;
}

const char* ArgumentParser::get_option_argument(const char* option_name) const
{
    const char** option = std::find_if(_argv, _argv + _argc, [option_name](const char* arg){ 
        return std::strcmp(arg, option_name) == 0; 
    });
    if (option < _argv + _argc - 1) return *(option + 1);
    return nullptr;
}
