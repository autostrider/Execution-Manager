#include "connection.hpp"
#include <i_client.hpp>
#include <i_server_socket.hpp>
#include <logger.hpp>

#include <fcntl.h>
#include <unistd.h>
#include <sys/un.h>

namespace base_server
{

Connection::Connection(std::shared_ptr<api::IServerSocket> serverSocket,
                       const int serverFd,
                       std::shared_ptr<api::IProxyClientFactory> proxyClientFactory)
    : m_serverSocket{serverSocket},
      m_serverFd{serverFd},
      m_clientFactory{proxyClientFactory},
      m_connectedCli{nullptr}
{}

int Connection::acceptConnection()
{
    sockaddr_un clientAddr;
    socklen_t cliLen = sizeof(clientAddr);

    int acceptFd =
        m_serverSocket->accept(m_serverFd,
                                (struct sockaddr*)&clientAddr,
                                &cliLen);

    if (acceptFd < 0 && errno == EINVAL)
    {
        LOG << "Server socket closed.";
        return acceptFd;
    }

    if (m_serverSocket->fcntl(acceptFd, F_SETFD, O_NONBLOCK) == -1 && errno == EINVAL)
    {
        acceptFd = -1;
        LOG << "Error in fcntl function: " << strerror(errno);
    }

    return acceptFd;
}

void Connection::creatAcceptedClient()
{
    int fd = acceptConnection();

    if (fd > 0)
    {
        m_connectedCli = m_clientFactory->makeProxyClient(fd);
    }
}

int Connection::receiveData(std::string& recv)
{ 
    return m_connectedCli->receive(recv);;
}

int Connection::getRecvBytes()
{
    return m_connectedCli->getRecvBytes();
}

void Connection::sendData(const google::protobuf::Message& context)
{
    m_connectedCli->sendMessage(context);
}

int Connection::getFd()
{
    return m_connectedCli->getClientFd();
}

}
