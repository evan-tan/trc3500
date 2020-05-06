#pragma once

#include <string>

std::string parity_check(std::string some_str)
{
    int count_ones;
    for (int i = 0; i<some_str.size()-1; i++)
    {
        if(some_str.substr(i,1) == "1")
        {
            count_ones++;
        }
    }

    if (count_ones % 2)
    {
        return "E";
    } else {
        return "O";
    }
}