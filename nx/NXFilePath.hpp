
#pragma once

#include "common.h"
//#include <sys/types.h>
//#include <dirent.h>

struct NXFilePath
{
private:
    char * _path = nullptr;
    U32    _len  = 0;
public:

    NXFilePath(const char * path)
    {
        if (!path)
            panic();

        _len = strlen(path);
        if (_len > 80)
            panic();

        if (path[0] != '/')
            panic();

        _path = (char *) malloc(1024);
        if (_path == NULL)
            panic();

        strcpy(_path, path);

        // Remove trailing slash
        if (_len > 2 && (_path[_len - 1] == '/'))
        {
            _path[_len - 1] = 0;
            _len--;
        }
    }

    ~NXFilePath()
    {
        if (_path)
        {
            free(_path);
            _path == nullptr;
        }

    }

    bool valid()
    {
        return (_path != nullptr);
    }


    // Delete the copy constructor and copy assignment operator
    NXFilePath (const NXFilePath&) = delete;
    NXFilePath& operator= (const NXFilePath&) = delete;

    // move constructor
    NXFilePath (NXFilePath&& other)
        : _path(other._path), _len (other._len)
    {
        other._path = nullptr;
        other._len  = 0;
    }
    // move assignment constructor
    NXFilePath& operator= (NXFilePath&& other)
    {
        if (_path)
            free(_path);
        _path = other._path;
        _len  = other._len;

        other._path = nullptr;
        other._len  = 0;
        return *this;
    }

    char * path()
    {
        return _path;
    }

    U32    len()
    {
        return _len;
    }

    NXFilePath add(char * path)
    {
        char buff[1024];
        strcpy(buff, _path);

        U32 plen = strlen(path);

        if ((_len + 1 + plen) >= 1024)
            panic();

        // Append the path separator
        buff[_len]     = '/';
        buff[_len + 1] = 0;
        strcat(buff, path);

        return NXFilePath{buff};
    }

    /**
     * basename - return the last element in the path
     * Example: Path '/abc/123' will return '123'
     */
    char * basename()
    {
        // The special case
        if (_len == 1)
            return _path;

        char * p = _path + (_len - 1);

        while (*p != '/')
            p--;

        return p;
    }

    /**
     * dirname - return the super directory element in the path
     * Example: Path '/abc/123' will return '/abc'
     */
    /* To Be Implemented */
};
