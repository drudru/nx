
#pragma once

#include "common.h"


// NXCStr
//
// A thin wrapper over C Strings
//
// These strings are immutable
//

struct NXCStr
{
    char * _str = nullptr;

    NXCStr() {}
    NXCStr(const char * str) { _str = (char *)str; }

    bool operator== (NXCStr & rhs)
    {
        if (_str == rhs._str)
            return true;

        if ((!_str) || (!rhs._str))
            return false;

        return (strcmp(_str, rhs._str) == 0);
    }

    bool operator!= (NXCStr & rhs)
    {
        return !(*this == rhs);
    }

    bool operator== (NXCStr && rhs)
    {
        if (_str == rhs._str)
            return true;

        if ((!_str) || (!rhs._str))
            return false;

        return (strcmp(_str, rhs._str) == 0);
    }
    bool operator!= (NXCStr && rhs)
    {
        return !(*this == rhs);
    }


    bool operator!()
    {
        return (_str == nullptr);
    }

    explicit operator bool()
    {
        return (_str != nullptr);
    }

    // Includes null byte
    U16 byte_count()
    {
        return (strlen(_str) + 1);
    }
};

