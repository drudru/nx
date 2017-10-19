
#pragma once

#include "common.h"

#include "NXPStr.hpp"
#include "NXProtoIterator.hpp"

struct NXStringTokenizer : NXProtoIterator<NXPStr>
{
private:
    char  * _start = nullptr;
    char  * _curr  = nullptr;
    bool    _done  = true;
public:

    NXStringTokenizer(char * start)
    {

        if (start == NULL)
            panic();

        _start = start;
        _curr = start;
        _done = false;
    }

    NXPStr get_next()
    {
        if (_done)
            panic();

        NXPStr result(_curr, 0);

        while (*_curr)
        {
            char c = *_curr;
            _curr++;
            if ( false
                 || (c == ' ')
                 || (c == '\n')
                 )
                break;
            result._len++;
        }

        if (*_curr == 0)
            _done = true;

        return result;
    }



    void rewind()
    {
        _curr = _start;
        _done = false;
    }

};

