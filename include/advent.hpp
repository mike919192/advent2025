
#pragma once

#include <exception>

namespace advt
{

inline int char_to_string(char in)
{
    if (in < '0' || in > '9')
        throw std::runtime_error("Char is not a number!");
    
    return in - '0';
}

}
