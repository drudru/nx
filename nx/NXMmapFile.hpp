
#pragma once

#include "common.h"
//#include <sys/types.h>
#include <sys/stat.h>


struct NXMmapFile
{
private:
    void * _p    = nullptr;
    int    _size = 0;
public:

    NXMmapFile()
    {
    }

    ~NXMmapFile()
    {
        if (_p)
            munmap(_p, _size);
    }

    bool map(const char * pathname)
    {
        int fd = open(pathname, O_RDONLY);
        if (fd < 0)
            return false;
    
        struct stat inode;
        {
            int res = fstat(fd, &inode);
            if (res < 0) panic();
        }
    
        _size = inode.st_size;
        // FIX - round up a page for null termination
        _p = mmap (0, _size, PROT_READ|PROT_WRITE, MAP_PRIVATE, fd, 0);
        if (_p == MAP_FAILED) panic();

        ((char *)_p)[_size] = 0;  // Null terminate

        close(fd);

        return true;
    }

    char * ptr()
    {
        return (char *)_p;
    }

    int   size()
    {
        return _size;
    }
};

