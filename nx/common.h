

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


// BREAKPOINTS

#if defined(__i386__) || defined(__x86_64__)

#define panic() __asm__("int $3");

#elif defined(__thumb__)

#define panic() __asm__ ("udf #1");
// This is equal to 0xde01 used by GDB as breakpoint

#elif defined(__arm__)

#define panic() __asm__ ("udf #0x10");
// This is equal to 0xe7f001f0 used by GDB as breakpoint

#else

#define panic() abort()

#endif



#define panic1(M) do { D("\33[31mPANIC\33[39m " __FILE__ ":" S(__LINE__) " - " M) ; abort(); } while(0);

