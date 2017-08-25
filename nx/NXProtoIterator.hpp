
#pragma once


template <typename T>
struct NXProtoIterator
{
    virtual T    get_next() = 0;
    virtual void rewind()   = 0;
};

