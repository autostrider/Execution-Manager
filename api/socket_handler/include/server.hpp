#ifndef SERVER_HPP
#define SERVER_HPP

#include "i_server.hpp"
#include "server_socket.hpp"
#include "client_connection.hpp"
#include "i_server_socket.hpp"
#include "connection.hpp"
#include <atomic>
#include <thread>
#include <vector>
#include <sys/un.h>
#include <mutex>


class Server : public IServer {
public:

    Server(const std::string &path, std::unique_ptr<IServerSocket> socket);

    ~Server() override;

    bool start() override;

    void stop() override;

private:
    std::unique_ptr<IServerSocket> m_server_socket;
    int m_server_fd = 0, m_addr_len = 0;
    struct sockaddr_un m_addr;
    std::atomic<bool> m_isRunning;
    std::string m_path;
    std::thread m_serverThread, m_receiveThread;
    std::vector<std::unique_ptr<Connection>> m_activeConnections;
    std::mutex m_mtx1;
    std::mutex m_mtx2;

    void bind();

    void listen();

    int accept();

    void handleConnections();

    void onRunning();
};

#endif //SERVER_HPP
