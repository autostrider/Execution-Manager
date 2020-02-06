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
#include <i_socket_server.hpp>
#include "i_socket_handler.hpp"

class SocketServer : public Socket//, ISocketServer
{
private:
    int  m_addr_len;
    struct sockaddr_un m_addr {};
    std::atomic<bool> m_isRunning ;
    std::string m_path;
    std::thread m_serverThread;
    std::vector<pollfd> m_activeConnections;
    std::queue<std::string> m_requests;
    std::unique_ptr<ISocket> m_socket;

public:

    explicit SocketServer(std::unique_ptr<ISocket> socket, const std::string& path);
    ~SocketServer();
    bool accept();
    bool run();
    void stop();
    ssize_t recvMessage(int fd, std::string &message) const;
    void onRunning();
};

#endif //EXECUTION_MANAGER_SOCKET_SERVER_HPP
