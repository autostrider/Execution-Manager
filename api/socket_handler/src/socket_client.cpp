#include "../include/socket_client.hpp"
#include <logger.hpp>
#include <string>
#include <constants.hpp>


SocketClient::SocketClient(const std::string &path) : Socket(), m_connected(false), m_path(path) {

    if (isCreated()) {
        m_addr.sun_family = AF_UNIX;
        path.copy(m_addr.sun_path, path.size() + 1);
        m_addr_len = sizeof(m_addr);
        if (m_fd < 0) {
            LOG << "Error opening socket";
        }
        if (::connect(m_fd, (const struct sockaddr *) &m_addr, m_addr_len) != -1) {
            m_connected = true;
        } else {
            LOG << "Client failed to connect socket";
        }
    }
}

bool SocketClient::isConnected()
{
    return m_connected;
}

SocketClient::~SocketClient() {
    if (::close(m_fd) == -1)
        LOG << "SocketClient: error on close()";
}

bool SocketClient::sendRequest(const std::string &message) {

    int sendBytes = send(m_fd, message.c_str(), message.size() + 1, MSG_NOSIGNAL);
    if (sendBytes == -1){
        LOG << "Error on sending message from client";
        return false;
    } else {
        return true;
    }
}

int SocketClient::recvMessage(int fd, std::string &message) const {

    char buffer[RECV_BUFFER_SIZE];

    int recvBytes = recv(fd, buffer, RECV_BUFFER_SIZE - 1, 0);
    if (recvBytes == -1)
        return -1;
    buffer[recvBytes] = 0;
    message.append(buffer);
    return recvBytes;;
}