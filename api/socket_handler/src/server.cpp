#include "server.hpp"

#include <algorithm>
#include <cerrno>
#include <logger.hpp>
#include <fcntl.h>
#include <mutex>
#include <constants.hpp>
#include <errno.h>
#include <unistd.h>

Server::Server(const std::string &path, std::unique_ptr<IServerSocket> socket)
        : m_server_socket{std::move(socket)},
          m_server_fd{m_server_socket->socket(AF_UNIX, SOCK_STREAM, 0)},
          m_isStarted{false},
          m_addr{},
          m_path{path},
          m_activeConnections{},
          m_serverThread{},
          m_receiveThread{},
          m_sQueue{},
          m_clientMaker{std::make_unique<ClientMaker>()}
{
    if (m_server_fd == -1)
    {
        LOG << " ServerSocket: Failed to create socket\n";
    }
    else
    {
        m_addr.sun_family = AF_UNIX;
        m_path.copy(m_addr.sun_path, m_path.size() + 1);
        m_addr_len = sizeof(m_addr);

        bind();
        listen();
    }
}

void Server::bind() {
    if (m_server_socket->bind(m_server_fd,
                              (struct sockaddr *) &m_addr,
                              m_addr_len) == -1)
    {
        LOG << "SocketServer: error on bind()\n";
    }
}

void Server::listen() {
    if (m_server_socket->listen(m_server_fd, 1) == -1)
    {
        LOG << "SocketServer: error on listen()\n";
    }
}

Server::~Server() {
    stop();
    if (m_server_socket->close(m_server_fd) == -1)
        LOG << "SocketServer: error on close()\n";

    ::unlink(m_addr.sun_path);
}

void Server::start() {
    m_isStarted = true;
    m_serverThread = std::thread(&Server::onRunning, this);
    m_receiveThread = std::thread(&Server::handleConnections, this);
}

void Server::onRunning() {
    m_activeConnections.reserve(NUMBER_RESERVED_CONNECTIONS);

    while (m_isStarted)
    {
        int clientFd = acceptConnection();

        if (clientFd > 0)
        {
            auto clientConnection = m_clientMaker->makeClientPtr(clientFd);;

            const std::lock_guard<std::mutex> guard(m_mtx1);
            m_activeConnections.push_back(clientConnection);
        }
        else if (clientFd < 0)
        {
            m_isStarted = false;
            LOG << "SocketServer: there is no clients connected\n";
        }
    }
}

int Server::acceptConnection()
{
    sockaddr_un clientAddr;
    socklen_t cliLen = sizeof(clientAddr);
    
    int fd =
        m_server_socket->accept(m_server_fd,
                                (struct sockaddr*)&clientAddr,
                                &cliLen);

    if (m_server_socket->fcntl(fd, F_SETFD, O_NONBLOCK) == -1)
    {
        return -1;
    }
    return fd;
}



void Server::handleConnections()
{
    while (m_isStarted) {
        for (auto it : m_activeConnections)
        {
            readFromSocket(it);
        }
    }
}

void Server::readFromSocket(std::shared_ptr<Client> connClient)
{
    std::string recv = connClient->receive();
    
    if (connClient->getRecvBytes() == 0)
    {        
        const std::lock_guard<std::mutex> lock(m_mtx2);
        
        m_activeConnections.erase(
            std::remove(m_activeConnections.begin(),
                        m_activeConnections.end(),
                        connClient),
            m_activeConnections.end());
    }
    else if (connClient->getRecvBytes() > 0)
    {
        m_sQueue.push(recv);
    }
}

void Server::stop() {
    m_isStarted = false;
    if (m_serverThread.joinable())
        m_serverThread.join();
    if (m_receiveThread.joinable())
        m_receiveThread.join();
}

std::string Server::getQueue()
{
    return m_sQueue.pop();
}

bool Server::isStarted()
{
    return m_isStarted;
}