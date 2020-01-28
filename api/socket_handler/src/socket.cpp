#include "../include/socket.hpp"
#include <unistd.h>
#include <sys/socket.h>
#include <iostream>
#include <logger.hpp>


Socket::Socket() : fd_created(false), temp_socket(0)
{
  if  ((fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
     LOG << " Failed to create client socket";
  }
      fd_created=true;
}

Socket::~Socket()
{
    if (fd) {
        ::close(fd);
    }
}

bool Socket::isOk()
{
    return fd_created;
}

void Socket::operator<<(std::string &to_send)
{
    send(temp_socket, to_send.c_str(), to_send.length(), 0);
}

void Socket::operator>>(std::string &received)
{
    char buffer[1024];
    int readed = 0, bytes;
    received = "";

    // receiving data
    bytes = recv(temp_socket, buffer, sizeof(buffer), 0);
    while (bytes > 0) {
        received += buffer;
        readed += bytes;
        if (bytes < sizeof(buffer))
            break;
        bytes=recv(temp_socket, buffer, sizeof(buffer), 0);
    }

    if (bytes)
        received = received.substr(0, readed);
    if (bytes == -1)
        received = "ERR";
    else
        received = "END";
}

void Socket::close()
{
    if (temp_socket) {
        ::close(temp_socket);
    }
    temp_socket=0;
}
