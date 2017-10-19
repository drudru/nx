
#pragma once

#include "common.h"

#include "NXNumBase.hpp"

// NXPStr
//
// A thin wrapper over Pascal type Strings
//
// These strings are immutable
// They only refer to another string
//

struct NXPStr
{
    char * _str = nullptr;
    U16    _len = 0;

    NXPStr() {}
    NXPStr(const char * str) { _str = (char *)str; _len = strlen(_str); }
    NXPStr(const char * str, U16 len) { _str = (char *)str; _len = len; }

    bool operator== (NXPStr & rhs)
    {
        if ((_len == rhs._len) && (_str == rhs._str))
            return true;

        if (_len != rhs._len)
            return false;

        // Compatible with memcmp
        if (_len == rhs._len && _len == 0)
            return true;

        // Lens are the same the pointers are different
        // Lens are > 0

        return (memcmp(_str, rhs._str, _len) == 0);
    }

    bool operator!= (NXPStr & rhs)
    {
        return !(*this == rhs);
    }

    bool operator== (NXPStr && rhs)
    {
        if ((_len == rhs._len) && (_str == rhs._str))
            return true;

        if (_len != rhs._len)
            return false;

        // Compatible with memcmp
        if (_len == rhs._len && _len == 0)
            return true;

        // Lens are the same the pointers are different
        // Lens are > 0

        return (memcmp(_str, rhs._str, _len) == 0);
    }

    bool operator!= (NXPStr && rhs)
    {
        return !(*this == rhs);
    }


    bool operator!()
    {
        return (_len == 0);
    }

    explicit operator bool()
    {
        return (_len != 0);
    }

    // Includes null byte
    U16 byte_count()
    {
        return _len;
    }

    int to_int(int base = 10)
    {
        int res = 0;

        bool negative = false;
        int i = 0;
        if (_str[i] == '-')
        {
            negative = true;
            i++;
        }

        for (; i < _len; i++)
        {
            U8 digit = NXNumBase::get_digit_from_ascii(_str[i], base);

            res *= base;
            res += digit;
        }

        if (negative)
            res = -res;

        return res;
    }

    void write(int fd)
    {
        ::write(fd, _str, _len);
    }
};

