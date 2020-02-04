#ifndef EXECUTION_MANAGER_SOCKET_SERVER_HPP
#define EXECUTION_MANAGER_SOCKET_SERVER_HPP

#include "socket.hpp"
#include <sys/socket.h>
#include <sys/un.h>
#include <atomic>
#include <thread>
#include <vector>
#include <poll.h>
#include <queue>

class SocketServer : public Socket
{
private:
    int  addr_un_len;
    struct sockaddr_un addr_un {};
    std::atomic<bool> isRunning ;
    std::string path;
    std::thread serverThread;
    std::vector<pollfd> activeConnections;
    std::queue<std::string> requests;

public:

    explicit SocketServer(const std::string& path);
    ~SocketServer();
    bool accept();
    bool run();
    void stop();
    ssize_t recvMessage(int fd, std::string &message) const;
    void onRunning();
};

#endif //EXECUTION_MANAGER_SOCKET_SERVER_HPP
