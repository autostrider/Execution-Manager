#ifndef SERVER_HPP
#define SERVER_HPP

#include <i_connection_factory.hpp>
#include <i_server.hpp>
#include <i_server_socket.hpp>
#include <safe_queue.hpp>

#include <atomic>
#include <mutex>
#include <thread>
#include <vector>
#include <sys/un.h>


class Server : public  api::socket_handler::IServer
{
public:
    Server(const std::string&, std::unique_ptr<IServerSocket>, std::unique_ptr<IConnectionFactory>);
    ~Server() override;

    void start() override;
    void stop() override;
    bool isStarted() override;
    void readFromSocket(std::shared_ptr<IConnection>) override;

    std::string getQueueElement();

private:

    void bind();
    void listen();

    void onRunning();
    void handleConnections();
    

private:
    std::shared_ptr<IServerSocket> m_server_socket;
    int m_server_fd = 0;
    int m_addr_len = 0;
    
    struct sockaddr_un m_addr;
    std::string m_path;

    std::atomic<bool> m_isStarted;

    std::vector<std::shared_ptr<IConnection>> m_activeConnections;

    std::thread m_serverThread;
    std::thread m_receiveThread;

    std::mutex m_mtxForServerThr;
    std::mutex m_mtxFroReceiveThr;

    SafeQueue m_recvDataQueue;

    std::shared_ptr<IConnectionFactory> m_connectionFactory;

};

#endif //SERVER_HPP