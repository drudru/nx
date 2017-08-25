
#pragma once

#include "common.h"
#include <stdio.h> 
#include <sys/types.h>
#include <dirent.h>

#include "NXFilePath.hpp"
#include "NXProtoIterator.hpp"

struct NXFileDir : NXProtoIterator<struct dirent *>
{
private:
    NXFilePath * _path = nullptr;
    DIR        * _pdir = nullptr;
    bool         _done = true;
public:

    NXFileDir(NXFilePath * path)
    {
        if (!path || (path->path() == NULL))
            panic();

        _path = path;
        _pdir = opendir(path->path());

        if (_pdir == NULL)
            panic();

        _done = false;
    }

    struct dirent * get_next()
    {
        if (_done)
            panic();

        struct dirent * pdirent = readdir(_pdir);

        if (pdirent == NULL)
            _done = true;
        //else
        //    printf(" dir: %s\n", pdirent->d_name);

        return pdirent;
    }

    void   rewind()
    {
        rewinddir(_pdir);
        _done = false;
    }

    ~NXFileDir()
    {
        if (_pdir)
            closedir(_pdir);
    }
};

struct NXHumanDir : NXProtoIterator<char *>
{
private:
    NXFileDir     * _pdir = nullptr;
    bool            _done = true;
    struct dirent * _pdirent = nullptr;
    char            _buff[18];
public:

    NXHumanDir(NXFileDir * pdir)
    {
        if (!pdir)
            panic();

        _pdir = pdir;
        _done = false;
    }

    char * get_next()
    {
        if (_done)
            panic();

        while (true)
        {
            _pdirent = _pdir->get_next();

            if (_pdirent == NULL)
            {
                _done = true;
                return NULL;
            }

            if ((_pdirent->d_name[0] == '.') || (_pdirent->d_name[0] == '_'))
            {
                continue;
            }

            _buff[17] =  0;
            _buff[16] = 15; // Asterisk like symbol to indicate name overflow
            switch (_pdirent->d_type)
            {
                case DT_REG:
                    _buff[0] = ' ';
                    break;
                case DT_DIR:
                    _buff[0] = '/';
                    break;
                default:
                    continue;
            }

            printf("Hdir: %s type: %d\n", _pdirent->d_name, _pdirent->d_type);

            int len = strlen(_pdirent->d_name);
            // memcpy(dst, src, len)
            if (len <= 15)
                memcpy(_buff + 1, _pdirent->d_name, len + 1); // copies null
            else
                memcpy(_buff + 1, _pdirent->d_name,      15); // does not copy null

            break;
        }

        return _buff;
    }

    void   rewind()
    {
        _pdir->rewind();
        _done = false;
        _pdirent = nullptr;
    }

    struct dirent * get_dirent()
    {
        return _pdirent;
    }
};

