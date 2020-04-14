#include "server.hpp"
#include "connection.hpp"
#include <constants.hpp>
#include <logger.hpp>

#include <algorithm>

namespace base_server
{

Server::Server(const std::string &path, std::unique_ptr<IServerSocket> socket, std::unique_ptr<IConnectionFactory> conn)
        : m_server_socket{std::move(socket)},
          m_server_fd{},
          m_isStarted{false},
          m_addr{},
          m_path{path},
          m_activeConnections{},
          m_serverThread{},
          m_receiveThread{},
          m_recvDataQueue{},
          m_connectionFactory{std::move(conn)}
{
    ::unlink(m_path.c_str());

    m_addr.sun_family = AF_UNIX;
    m_path.copy(m_addr.sun_path, m_path.size() + 1);
    
    createSocket();
    bind();
    listen();
}

void Server::createSocket()
{
    m_server_fd = m_server_socket->socket(AF_UNIX, SOCK_STREAM, 0);

    if (m_server_fd == -1)
    {
        LOG << "Failed to create socket: "
            <<strerror(errno) << ".";
    }
}

void Server::bind()
{
    if (m_server_socket->bind(m_server_fd,
                              (struct sockaddr *) &m_addr,
                              sizeof(m_addr)) == (-1))
    {
        LOG << "Error on bind function: "
            << strerror(errno) << ".";
    }
}

void Server::listen()
{
    if (m_server_socket->listen(m_server_fd, 1) == (-1))
    {
        LOG << "Error on listen function: "
            << strerror(errno) << ".";
    }
}

void Server::start()
{
    m_isStarted = true;
    m_serverThread = std::thread(&Server::onRunning, this);
    m_receiveThread = std::thread(&Server::handleConnections, this);
}

void Server::onRunning()
{
    while (m_isStarted)
    {
        std::shared_ptr<IConnection> connection =
                m_connectionFactory->makeConnection(m_server_socket, m_server_fd);

        if (connection)
        {
            connection->creatAcceptedClient();

            const std::lock_guard<std::mutex> guard(m_mtxForServerThr);
            m_activeConnections.push_back(connection);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
}

void Server::handleConnections()
{
    while (m_isStarted)
    {
        {
            const std::lock_guard<std::mutex> guard(m_mtxForServerThr);
            for (auto it = m_activeConnections.begin(); it < m_activeConnections.end(); it++)
            {
                readFromSocket(it);
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
}

void Server::readFromSocket(std::vector<std::shared_ptr<IConnection>>::iterator& conn)
{
    std::string recv;
    int byte = (*conn)->receiveData(recv);

    if (byte == 0)
    {
        conn = m_activeConnections.erase(
            std::remove(m_activeConnections.begin(),
                        m_activeConnections.end(),
                        *conn),
            m_activeConnections.end());
    }
    else if (byte > 0)
    {
        m_recvDataQueue.push({recv, (*conn)->getFd()});
    }
}

void Server::stop()
{
    m_isStarted = false;
    closeSocketConnection();

    if (m_serverThread.joinable())
        m_serverThread.join();
    if (m_receiveThread.joinable())
        m_receiveThread.join();
}

void Server::closeSocketConnection()
{
    if (m_server_socket->shutdown(m_server_fd) == (-1))
        LOG << "Error on shutdown function: "
            << strerror(errno) << ".";
}

void Server::send(const google::protobuf::Message& context, int fd)
{
    if (m_isStarted)
    {
        auto it = std::find_if(m_activeConnections.cbegin(), m_activeConnections.cend(), [fd] (const auto& activeConnection)
        {
            return activeConnection->getFd() == fd;
        });

        if (it != m_activeConnections.cend())
        {
            (*it)->sendData(context);
        }
        else
        {
            LOG << "Error proxy client does not exist fd: " << fd;
        }
        
    }
}

bool Server::getQueueElement(common::ReceivedMessage& data)
{
    return m_recvDataQueue.pop(data);
}

bool Server::isStarted()
{
    return m_isStarted;
}

}
