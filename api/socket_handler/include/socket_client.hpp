#ifndef EXECUTION_MANAGER_SOCKET_CLIENT_HPP
#define EXECUTION_MANAGER_SOCKET_CLIENT_HPP


#include "socket.hpp"
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>

class SocketClient : public Socket
{
private:
    struct sockaddr_un m_addr{};
    int  m_addr_len;
    bool  m_connected;
    std::string m_path;


public:
    explicit SocketClient(const std::string& path);
    ~SocketClient();
    bool isConnected();
    bool sendRequest(const std::string &message);
    int recvMessage(int fd, std::string &message) const ;

};

#endif //EXECUTION_MANAGER_SOCKET_CLIENT_HPP
