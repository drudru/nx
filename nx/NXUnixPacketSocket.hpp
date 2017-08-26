

#pragma once

#include "common.h"
#include <sys/types.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <poll.h>

#include "NXCStr.hpp"
#include "NXFilePath.hpp"


struct NXUnixPacketSocket
{
    int _fd     = -1;
    int _listen = false;

    U8  _buffer[16];

    NXUnixPacketSocket() {}

    ~NXUnixPacketSocket()
    {
        reset();
    }

    // free the old if necessary
    void reset()
    {
        if (valid())
        {
            close(_fd);
            _fd = -1;
        }
    }

    // Release ownership to another instance
    int release()
    {
        int fd = _fd;
        _fd = -1;
        return fd;
    }

    bool valid()
    {
        return (_fd != -1);
    }

    // Delete the copy constructor and copy assignment operator
    NXUnixPacketSocket (const NXUnixPacketSocket&) = delete;
    NXUnixPacketSocket& operator= (const NXUnixPacketSocket&) = delete;

    // move constructor
    NXUnixPacketSocket (NXUnixPacketSocket&& other)
        : _fd(other._fd), _listen (other._listen)
    {
        other.release();
    }
    // move assignment constructor
    NXUnixPacketSocket& operator= (NXUnixPacketSocket&& other)
    {
        if (_fd == other._fd)
            panic();

        reset();

        _fd      = other._fd;
        _listen  = other._listen;

        other.release();

        return *this;
    }

    static
    NXUnixPacketSocket CreateServer(NXFilePath path)
    {
        if (!path.valid())
            panic(); // "invalid arg"

        int fd;
        struct sockaddr_un addr;

        if ( (fd = socket(AF_UNIX, SOCK_SEQPACKET|SOCK_CLOEXEC, 0)) == -1)
            panic(); // "socket creation error"

        char * socket_path = path.path();

        if (path.len() >= sizeof(addr.sun_path))
            panic(); // "path for unix socket too long"

        memset(&addr, 0, sizeof(addr));
        addr.sun_family = AF_UNIX;
        strncpy(addr.sun_path, socket_path, sizeof(addr.sun_path) - 1);

        // Remove the old before we can re-bind
        unlink(socket_path);

        if (bind(fd, (struct sockaddr*)&addr, sizeof(addr)) == -1)
            panic(); // "socket bind error"

        /*
           int flags = fcntl(fd, F_GETFL, 0);
           if (flags < 0)
           panic("socket fcntl get error");
           if (fcntl(fd, F_SETFL, flags | O_NONBLOCK))
           panic("socket fcntl set error");
           */

        NXUnixPacketSocket s;
        s._fd     = fd;
        s._listen = true;

        return s;
    }

    static
    NXUnixPacketSocket CreateClient(NXFilePath path)
    {
        if (!path.valid())
            panic(); // "invalid arg"

        int fd;
        struct sockaddr_un addr;

        if ( (fd = socket(AF_UNIX, SOCK_SEQPACKET|SOCK_CLOEXEC, 0)) == -1)
            panic(); // "socket creation error"

        char * socket_path = path.path();

        if (path.len() >= sizeof(addr.sun_path))
            panic(); // "path for unix socket too long"

        memset(&addr, 0, sizeof(addr));
        addr.sun_family = AF_UNIX;
        strncpy(addr.sun_path, socket_path, sizeof(addr.sun_path) - 1);

        if (connect(fd, (struct sockaddr*)&addr, sizeof(addr)) == -1)
        {
            if (errno == ECONNREFUSED)
            {
                close(fd);
                fd = -1;
            }
            else
                panic(); // "socket connect error";
        }

        NXUnixPacketSocket s;

        s._fd     = fd;
        s._listen = false;

        return s;
    }

    void listen(int backlog = 64)
    {
        if (!_listen)
            panic();

        if (::listen(_fd, backlog) == -1)
            panic();
    }

    NXUnixPacketSocket accept()
    {
        if (!_listen)
            panic();

        int new_fd = ::accept(_fd, NULL, NULL);
        if (new_fd == -1)
            panic();

        NXUnixPacketSocket s;

        s._fd     = new_fd;
        s._listen = false;

        return s;
    }

    // Poll the fd to see if it is readable
    bool readable()
    {
        struct pollfd pfd = {
            .fd      = _fd,
            .events  = POLLIN,
            .revents = 0
        };

        int ret = poll(&pfd, 1, 0);
        if (ret == -1)
            panic();

        return (ret == 1);
    }

    void send_msg(NXCStr msg)
    {
        if (_listen)
            panic();

        U16 num_bytes = msg.byte_count();

        if (num_bytes > sizeof(_buffer))
            panic();

        int ret = write(_fd, msg._str, num_bytes);
        if (ret != num_bytes)
            panic();
    }

    NXCStr recv_msg()
    {
        if (_listen)
            panic();

        int ret = read(_fd, _buffer, sizeof(_buffer));
        if (ret == -1)
            panic();
        if (ret == 0)
            return "";

        return NXCStr((const char *)_buffer);
    }

    void send_ack()
    {
        send_msg("ack");
    }

    // wait for 'ack' or an EOF
    void recv_ack()
    {
        auto msg = recv_msg();
        if ((msg == "ack") || (msg == ""))
            return;

        panic();
    }
};

