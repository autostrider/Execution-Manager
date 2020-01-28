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
    bool ready, connected;
    std::string path;

    void assignSocket(const std::string& path);

public:
   explicit SocketClient(const std::string& path);
  
    void connect();
    bool isReady();
    bool isConnected();
};

#endif //EXECUTION_MANAGER_SOCKET_CLIENT_HPP
