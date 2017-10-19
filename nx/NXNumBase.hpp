
#pragma once

#include "common.h"


// NXNumBase
//
// Convert a digit from ascii to an intenger 
// or vice versa in a specific base
//

char table[] = "0123456789" "ABCDEF";

struct NXNumBase
{
    static const U8 MAX_U32_STR_B10 = 10; 
    static const U8 MAX_U32_STR_B16 =  8;

    static U8 get_digit_from_ascii(U8 ascii, U8 base)
    {
        if (base > 16) panic();

        U8 result;

        if ((ascii >= '0') && (ascii <= '9'))
            result = ascii - '0';
        else
        if ((ascii >= 'a') && (ascii <= 'f'))
            result = 10 + ascii - 'a';
        else
        if ((ascii >= 'A') && (ascii <= 'F'))
            result = 10 + ascii - 'A';
        else
            panic();

        if (result >= base)
            panic();

        return result;
    }

    static void get_ascii_from_int(U32 num, U8 base, char * buff)
    {
        if (base > 16) panic();

        int curr_len = 0;

        //buff[1] = '0';
        buff[0] =  0 ;

        while (num != 0)
        {
            curr_len++;
            for (int i = curr_len; i != 0; i--)
                buff[i] = buff[i - 1];

            buff[0]  = table[num % base];
            num     /= base;
        }
    }
};

