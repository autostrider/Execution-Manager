#ifndef EXECUTION_MANAGER_SOCKET_SERVER_HPP
#define EXECUTION_MANAGER_SOCKET_SERVER_HPP

#include "socket.hpp"
#include <sys/socket.h>
#include <sys/un.h>

class SocketServer : public Socket
{
private:
    int server_socket, addr_un_len;
    struct sockaddr_un addr_un;
    bool connected;

    void assignSocket(const std::string& path);

public:
    explicit SocketServer(const std::string& path);
    bool isConnected();
    bool accept();
};

#endif //EXECUTION_MANAGER_SOCKET_SERVER_HPP
