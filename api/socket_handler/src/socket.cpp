#include "../include/socket.hpp"
#include <unistd.h>
#include <sys/socket.h>


Socket::Socket() : m_fdcreated(false)
{
  if  ((m_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
     LOG << " Failed to create socket";
  }
  else {
      m_fdcreated = true;
  }
}

Socket::~Socket()
{
   if(::close(m_fd) == -1)
    LOG << "Socket error on close()";
}

bool Socket::isCreated()
{
    return m_fdcreated;
}
