#include "../include/socket_client.hpp"
#include <logger.hpp>
#include <string>
#include <constants.hpp>


SocketClient::SocketClient(const std::string &path) : Socket(), connected(false), path(path) {

    if (isCreated()) {
        addr_un.sun_family = AF_UNIX;
        path.copy(addr_un.sun_path, path.size() + 1);
        addr_un_len = sizeof(addr_un);
        if (fd < 0) {
            LOG << "Error opening socket";
        }
        if (::connect(fd, (const struct sockaddr *) &addr_un, addr_un_len) != -1) {
            connected = true;
        } else {
            LOG << "Client failed to connect socket";
        }
    }
}

bool SocketClient::isConnected()
{
    return connected;
}

SocketClient::~SocketClient() {
    if (::close(fd) == -1)
        LOG << "SocketClient: error on close()";
}

bool SocketClient::sendRequest(const std::string &message) {

    int sendBytes = send(fd, message.c_str(), message.size() + 1, MSG_NOSIGNAL);
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