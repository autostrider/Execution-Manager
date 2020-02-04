#ifndef EXECUTION_MANAGER_SOCKET_CLIENT_HPP
#define EXECUTION_MANAGER_SOCKET_CLIENT_HPP


#include "socket.hpp"
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>
#include <cstring>

class SocketClient : public Socket
{
private:
    struct sockaddr_un addr_un{};
    int  addr_un_len;
    bool  connected;
    std::string path;


public:
    explicit SocketClient(const std::string& path);
    ~SocketClient();
    bool isConnected();
    bool sendRequest(const std::string &message);
    int recvMessage(int fd, std::string &message) const ;

};

#endif //EXECUTION_MANAGER_SOCKET_CLIENT_HPP
