#include "../include/socket_client.hpp"
#include <logger.hpp>
#include <string>



SocketClient::SocketClient(const std::string &path) : Socket(), connected(false), path(path) {

    if (isCreated()) {
        addr_un.sun_family = AF_UNIX;
        std::strcpy(addr_un.sun_path, path.c_str());
        addr_un_len = std::strlen(addr_un.sun_path) + sizeof(addr_un.sun_family);
        if (fd < 0) {
            LOG << "Error opening socket ";
        }
        if (::connect(fd, (const struct sockaddr *) &addr_un, addr_un_len) != -1) {
            temp_socket = fd;
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

    std::string messageRequest =  message;

    int sendBytes = send(fd, messageRequest.c_str(), messageRequest.size() + 1, MSG_NOSIGNAL);
    if (sendBytes == -1){
        LOG << "Error on sending message from client";
        return false;
    } else {
        return true;
    }
}