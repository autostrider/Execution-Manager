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

    int accepdFd =
        m_serverSocket->accept(m_serverFd,
                                (struct sockaddr*)&clientAddr,
                                &cliLen);

    if (m_serverSocket->fcntl(accepdFd, F_SETFD, O_NONBLOCK) == -1 && !EBADF)
    {
        accepdFd = -1;
        LOG << "Error in fcntl function: " << strerror(errno);
    }

    return accepdFd;
}

void Connection::creatAcceptedClient()
{
    int fd = acceptConnection();

    if (fd > 0)
    {
        m_connectedCli = m_clientFactory->makeProxyClient(fd);
    }
    else if (fd < 0)
    {
        LOG << "There is no clients connected.";
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

}
