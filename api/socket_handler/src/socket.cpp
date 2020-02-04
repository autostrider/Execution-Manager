#include "../include/socket.hpp"
#include <unistd.h>
#include <sys/socket.h>


Socket::Socket() : fd_created(false)//, temp_socket(0)
{
  if  ((fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
     LOG << " Failed to create socket";
  }
  else {
      fd_created = true;
  }
}

Socket::~Socket()
{
   if(::close(fd) == -1)
    LOG << "Socket error on close()";
}

bool Socket::isCreated()
{
    return fd_created;
}
