#include "client.hpp"
#include <constants.hpp>
#include <logger.hpp>
#include <any.pb.h>

using namespace constants;

namespace base_client
{

Client::Client(const std::string &path, std::unique_ptr<IClientSocket> socket)
        : m_client_socket{std::move(socket)},
          m_path{path},
          m_connected{false},
          m_addr{}
{
    m_addr.sun_family = AF_UNIX;
    m_path.copy(m_addr.sun_path, m_path.size() + 1);
    m_addr_len = (sizeof(m_addr));

    createSocket();
}

void Client::createSocket()
{
    m_client_fd = m_client_socket->socket(AF_UNIX, SOCK_STREAM, 0);

    if (m_client_fd < 0)
    {
        LOG << "Error opening socket\n";
    }
}

void Client::connect()
{
    if (0 == m_client_socket->connect(m_client_fd,
                                      (const struct sockaddr *) &m_addr,
                                      m_addr_len))
    {
        m_connected = true;
    }
    else
    {
        LOG << "Client failed to connect socket: " << errno;
    }
}

Client::~Client()
{
    m_client_socket->close(m_client_fd);
}

ssize_t Client::receive(std::string& recvMessage)
{
    char buffer[RECV_BUFFER_SIZE];

    ssize_t byte = m_client_socket->recv(m_client_fd, buffer, RECV_BUFFER_SIZE - 1, MSG_DONTWAIT);
    
    if (errno == EWOULDBLOCK && byte == -1)
    {
        return byte;
    }
    else if(byte == -1)
    {
        LOG << "Error on receiving data from client: " << strerror(errno);
    }

    recvMessage = buffer;

    return byte;
}

void Client::sendMessage(const google::protobuf::Message& context)
{
    google::protobuf::Any message;
    message.PackFrom(context);

    size_t size = message.ByteSizeLong();
    std::unique_ptr<uint8_t> dataPtr = std::unique_ptr<uint8_t>(new uint8_t[size]);
    auto data = dataPtr.get();
        
    if(message.SerializeToArray(data, size))
    {
        m_client_socket->send(m_client_fd, data, size, 0);
    }
}

bool Client::isConnected()
{
    return m_connected;
}

void Client::setClientFd(int fd)
{
    m_client_fd = fd;
}

void Client::setConnected(bool connected)
{
    m_connected = connected;
}

int Client::getClientFd()
{
    return m_client_fd;
}

ssize_t Client::getRecvBytes()
{
    return m_recvBytes;
}

}
