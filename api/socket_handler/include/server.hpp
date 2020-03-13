#ifndef SERVER_HPP
#define SERVER_HPP

#include "i_server.hpp"
#include "server_socket.hpp"
#include "client_socket.hpp"
#include "i_server_socket.hpp"
#include "client.hpp"
#include <string_safe_queue.hpp>
#include <client_factory.hpp>

#include <atomic>
#include <mutex>
#include <thread>
#include <vector>
#include <sys/un.h>

class Server : public IServer {
public:

    Server(const std::string&, std::unique_ptr<IServerSocket>);

    ~Server() override;

    void start() override;
    void stop() override;
    bool isStarted() override;
    int acceptConnection() override;
    void readFromSocket(std::shared_ptr<IClient>) override;

    std::string getQueue();

private:

    void bind();
    void listen();

    void onRunning();
    void handleConnections();
    

private:
    std::unique_ptr<IServerSocket> m_server_socket;
    int m_server_fd = 0, m_addr_len = 0;
    
    struct sockaddr_un m_addr;
    std::string m_path;

    std::atomic<bool> m_isStarted;

    std::vector<std::shared_ptr<IClient>> m_activeConnections;

    std::thread m_serverThread, m_receiveThread;
    std::mutex m_mtx1;
    std::mutex m_mtx2;

    StringSafeQueue m_sQueue;

    std::unique_ptr<IClientFactory> m_clientFactory;
};

#endif //SERVER_HPP
