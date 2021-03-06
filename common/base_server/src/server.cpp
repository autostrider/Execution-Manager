#include "server.hpp"
#include "connection.hpp"
#include <constants.hpp>
#include <logger.hpp>

namespace base_server
{

Server::Server(const std::string &component, std::unique_ptr<IServerSocket> socket, std::unique_ptr<IConnectionFactory> conn)
        : m_server_socket{std::move(socket)},
          m_server_fd{},
          m_isStarted{false},
          m_addr{},
          m_componentName{component},
          m_activeConnections{},
          m_serverThread{},
          m_receiveThread{},
          m_recvDataQueue{},
          m_connectionFactory{std::move(conn)}
{
    auto path = constants::COMPONENT_SOCKET_NAME + "_" + m_componentName;

    ::unlink(path.c_str());

    m_addr.sun_family = AF_UNIX;
    path.copy(m_addr.sun_path, path.size() + 1);
    
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

Server::~Server()
{
    stop();
    if (m_server_socket->close(m_server_fd) == (-1))
        LOG << "Error on close function: "
            << strerror(errno) << ".";
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

        connection->creatAcceptedClient();
        {
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
            for (auto it : m_activeConnections)
            {
                readFromSocket(it);
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
}

void Server::readFromSocket(std::shared_ptr<IConnection> conn)
{
    std::string recv = conn->receiveData();

    if (conn->getRecvBytes() == 0)
    {
        m_activeConnections.erase(
            std::remove(m_activeConnections.begin(),
                        m_activeConnections.end(),
                        conn),
            m_activeConnections.end());
    }
    else if (conn->getRecvBytes() > 0)
    {
        m_recvDataQueue.push(recv);
    }
}

void Server::stop()
{
    m_isStarted = false;
    if (m_serverThread.joinable())
        m_serverThread.join();
    if (m_receiveThread.joinable())
        m_receiveThread.join();
}

bool Server::getQueueElement(std::string& data)
{
    return m_recvDataQueue.pop(data);
}

bool Server::isStarted()
{
    return m_isStarted;
}

}