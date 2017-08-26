

#pragma once

#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdint.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

typedef uint8_t  U8;
typedef uint16_t U16;
typedef uint32_t U32;
typedef int8_t   I8;
typedef int16_t  I16;

#define D(M) if (DEBUG) { DBG_WRITE(M "\n") }


#define DBG_WRITE(M) \
    do {\
        const char * pm = M;\
        int len = strlen(pm);\
        write(2, pm, len);\
    } while(0);

// Does not check for error on write to stderr


#define S(x) STRINGIZE2(x)
#define STRINGIZE2(x) #x

#define panic abort
#define panic1(M) do { D("\33[31mPANIC\33[39m " __FILE__ ":" S(__LINE__) " - " M) ; abort(); } while(0);

