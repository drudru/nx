
#pragma once

#include "common.h"

#include "NXPStr.hpp"
#include "NXProtoIterator.hpp"

struct NXStringTokenizer : NXProtoIterator<NXPStr>
{
private:
    const char  * _start = nullptr;
    char        * _curr  = nullptr;
    bool          _done  = true;
public:

    NXStringTokenizer(const char * start)
    {

        if (start == NULL)
            panic();

        _start = start;
        _curr = (char *)start;
        _done = false;
    }

    bool is_done()
    {
        return _done;
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
        _curr = (char *)_start;
        _done = false;
    }

};

