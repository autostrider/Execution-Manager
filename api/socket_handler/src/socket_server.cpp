#include "../include/socket_server.hpp"
#include "../../../common/include/constants.hpp"
#include <iostream>
#include <zconf.h>
#include <fcntl.h>


SocketServer::SocketServer(const std::string &path) : Socket(), isRunning(false), path(path) {
    if (isCreated()) {
        addr_un.sun_family = AF_UNIX;
        path.copy(addr_un.sun_path, path.size() + 1);
        addr_un_len = sizeof(addr_un);
        if (::bind(fd, (struct sockaddr *) &addr_un, addr_un_len) == -1)
            perror("error on bind()");
        if (::listen(fd, 1) == -1)
            perror("error on listen()");
    }
}

SocketServer::~SocketServer()
{
    stop();
    if (::close(fd) == -1)
        perror("error on close()");
    if (unlink(path.c_str()) == -1)
        perror(" error on remove()");
}



bool SocketServer::accept() {
    temp_socket = ::accept(fd, (struct sockaddr *) &addr_un, (socklen_t *) &addr_un_len);

    if (temp_socket < 0) {
        perror("error on accept()");
        return false;
    }
    if (fcntl(temp_socket, F_SETFL, O_NONBLOCK) == -1) {
        perror("error on fcntl()");
        return false;
    }
    activeConnections.push_back({temp_socket, POLLIN, 0});
    return true;
}

ssize_t SocketServer::recvMessage(int fd, std::string &message) const {
    char buffer[RECV_BUFFER_SIZE];
    int recvBytes = recv(fd, buffer, RECV_BUFFER_SIZE - 1, 0);
    if (recvBytes == -1)
        return -1;
    buffer[recvBytes] = 0;
    message.append(buffer);
    return recvBytes;
}

bool SocketServer::run() {
    if (isRunning)
        return false;
    isRunning = true;
    serverThread = std::thread(&SocketServer::onRunning, this);
    return true;
}


void SocketServer::stop() {
    isRunning = false;
    if (serverThread.joinable())
        serverThread.join();
}

void SocketServer::onRunning() {
    activeConnections.reserve(NUMBER_RESERVED_CONNECTIONS);
    activeConnections.push_back({fd, POLLIN, 0});

    while (isRunning){
        int   poolResult = poll(
                activeConnections.data(), activeConnections.size(), TIMEOUT_FOR_SOCKET);
        switch (poolResult) {
            case -1:
                perror("error on poll()");
                return;
            case 0:// timeout
                continue;
            default:
                break;
        }
        if (activeConnections[0].revents & POLLIN) {
            activeConnections[0].revents = 0;
            --poolResult;
            accept();
            if (accept())
                std::cout << "Client connected successfully";

        }

        for (auto it = activeConnections.begin() + 1; it != activeConnections.end() && poolResult > 0; ) {
            if (it->revents & POLLIN) {
                --poolResult;
                it->revents = 0;

                std::string message;
                ssize_t recvBytes = recvMessage(it->fd, message);

                if (recvBytes < 1) {
                    perror("error on receiving data from client");
                    ::close(it->fd);
                    it = activeConnections.erase(it);
                    continue;
                } else {
                    std::string response;
                    if (!message.empty()) {
                        int sendBytes = ::send(it->fd, message.c_str(), message.size() + 1, MSG_NOSIGNAL);
                        if (sendBytes == -1)
                           perror(" error on sending the response");
                    }
                }
            }
            it++;
        }

    }
}