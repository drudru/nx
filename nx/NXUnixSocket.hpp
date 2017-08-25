


#include "common.h"
#include <sys/types.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/un.h>


struct NXUnixSocket
{
  int _fd     = -1;
  int _server = false;

  NXUnixSocket() {}

  ~NXUnixSocket()
  {
      if (_fd != -1)
      {
          close(_fd);
          _fd = -1;
      }
  }

  static
  NXUnixSocket CreateServer(Path & path)
  {
      if (!path.valid())
          panic("invalid arg");

      int fd;
      struct sockaddr_un addr;

      if ( (fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1)
          panic(); // "socket creation error"

      char * socket_path = path._path;

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

      NXUnixSocket s;
      s._fd     = fd;
      s._server = true;

      return s;
  }

  static
  NXUnixSocket CreateClient(Path path)
  {
      if (!path.valid())
          panic("invalid arg");

      int fd;
      struct sockaddr_un addr;

      if ( (fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1)
          panic(); // "socket creation error"

      char * socket_path = path._path;

      if (path.len() >= sizeof(addr.sun_path))
          panic(); // "path for unix socket too long"

      memset(&addr, 0, sizeof(addr));
      addr.sun_family = AF_UNIX;
      strncpy(addr.sun_path, socket_path, sizeof(addr.sun_path) - 1);

      if (connect(fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
          if (errno == ECONNREFUSED)
          {
              close(fd);
              fd = -1;
          }
          else
              panic(); // "socket connect error";
      }

      NXUnixSocket s;
      s._fd     = fd;
      s._server = false;

      return s;
  }

  void listen(int backlog = 64)
  {
    if (!_server)
        panic();

    if (listen(_fd, backlog) == -1)
        panic();
  }

  int take_fd()
  {
      if (_fd == -1)
          panic();

      int fd = _fd;
      _fd = -1;

      return fd;
  }
};

