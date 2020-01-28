#include "../include/socket_server.hpp"
#include <sys/un.h>
#include <iostream>

SocketServer::SocketServer(const std::string& path) : Socket(), connected(false) {
    assignSocket(path);
}

void SocketServer::assignSocket(const std::string& path)
{
    if (isOk()) {
                addr_un.sun_family = AF_UNIX;
            if (bind(fd, (struct sockaddr*) &addr_un, sizeof(addr_un)) != -1) {

                addr_un_len = sizeof(addr_un);

                if (listen(fd, 1) != -1)
                    connected=true;
            }
        }
}

bool SocketServer::isConnected()
{
    return connected;
}

bool SocketServer::accept()
{
    temp_socket = ::accept(fd, (struct sockaddr *) &addr_un, (socklen_t *) &addr_un_len);
    return temp_socket != -1;
}

