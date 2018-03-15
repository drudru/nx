
#pragma once

#include "common.h"


struct NXMSDOSHeader
{
    NXMmapFile * data   = nullptr;
    bool         valid  = false;
    bool         ne_fmt = false;
    bool         pe_fmt = false;

    U32          lfanew = 0;

    NXMSDOSHeader()
    {
    }

    void init()
    {
        if ((data->ptr()[0] != 'M') || 
            (data->ptr()[1] != 'Z'))
            return;

        // Handle other fields in the future

        // Reloc_pos
        if (data->ptr()[0x18] != 0x40)
            return;

        lfanew = (U8) data->ptr()[0x3c];

        // 'SEGMENTED EXE HEADER'
        if ((data->ptr()[lfanew + 0] == 'N') && 
            (data->ptr()[lfanew + 1] == 'E'))
        {
            valid = true;
            ne_fmt = true;
            return;
        }

        if ((data->ptr()[lfanew + 0] == 'P') && 
            (data->ptr()[lfanew + 1] == 'E'))
        {
            valid = true;
            pe_fmt = true;
            return;
        }
    }
};

