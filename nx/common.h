

#pragma once

#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdint.h>
#include <stdlib.h>

#define panic abort

typedef uint8_t  U8;
typedef uint16_t U16;
typedef uint32_t U32;
typedef int8_t   I8;
typedef int16_t  I16;
