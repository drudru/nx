


#pragma once

#include "common.h"

// A convenience class that
// safely traverses an in-memory
// stream
//
// Also does your typical PACK/UNPACK
// operations on binary data
//

struct NXMemStream
{
    NXMmapFile * data;

    bool         little_endian = true;
    U8         * start;
    U8         * current;
    U8         * last;

    void init()
    {
        if (!data)
            panic();
        if (data->size() == 0)
            panic();

        // TODO: handle endian
        //


        start   = (U8 *) data->ptr();
        current = start;
        last    = current + data->size() - 1;
    }

    void skip(U32 bytes)
    {
        current += bytes;
        if (current > last)
            panic();
    }

    void abs_pos(U32 bytes)
    {
        current = start;
        skip(bytes);
    }

    U16 readU16()
    {
        U16 result = 0;

        if (little_endian)
        {
            result = *current;
            skip(1);
            result |= (*current) << 8;
            skip(1);
        }
        return result;
    }

};

