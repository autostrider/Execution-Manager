#include "../include/socket_server.hpp"
#include <constants.hpp>
#include <iostream>
#include <zconf.h>
#include <fcntl.h>

SocketServer::SocketServer(std::unique_ptr<ISocket> socket, const std::string &path) : Socket(), m_socket(std::move(socket)), m_isRunning(false), m_path(path) {
    if (isCreated()) {
        m_addr.sun_family = AF_UNIX;
        path.copy(m_addr.sun_path, path.size() + 1);
        m_addr_len = sizeof(m_addr);
        if (m_socket->bind(m_fd, (struct sockaddr *) &m_addr, m_addr_len) == -1)
            LOG << "SocketServer: error on bind()";
        if (m_socket->listen(m_fd, 1) == -1)
            LOG << "SocketServer: error on listen()";
    }
}

SocketServer::~SocketServer()
{
    stop();
    if (m_socket->close(m_fd) == -1)
      LOG << "SocketServer: error on close()";
    if (::unlink(m_path.c_str()) == -1)
        LOG << " SocketServer: error on unlink()";
}



bool SocketServer::accept() {
   int temp_socket = m_socket->accept(m_fd, (struct sockaddr *) &m_addr, (socklen_t *) &m_addr_len);

    if (temp_socket < 0) {
        LOG << "SocketServer: error on accept()";
        return false;
    }
    if (fcntl(temp_socket, F_SETFL, O_NONBLOCK) == -1) {
        LOG << "SocketServer: error on fcntl()";
        return false;
    }
    m_activeConnections.push_back({temp_socket, POLLIN, 0});
    return true;
}

ssize_t SocketServer::recvMessage(int fd, std::string &message) const {
    char buffer[RECV_BUFFER_SIZE];
    int recvBytes = m_socket->recv(fd, buffer, RECV_BUFFER_SIZE - 1, 0);
    if (recvBytes == -1)
        return -1;
    buffer[recvBytes] = 0;
    message.append(buffer);
    return recvBytes;
}

bool SocketServer::run() {
    if (m_isRunning)
        return false;
    m_isRunning = true;
    m_serverThread = std::thread(&SocketServer::onRunning, this);
    return true;
}


void SocketServer::stop() {
    m_isRunning = false;
    if (m_serverThread.joinable())
        m_serverThread.join();
}

void SocketServer::onRunning() {
    m_activeConnections.reserve(NUMBER_RESERVED_CONNECTIONS);
    m_activeConnections.push_back({m_fd, POLLIN, 0});

    while (m_isRunning){
        int   poolResult = poll(
                m_activeConnections.data(), m_activeConnections.size(), SERVER_TIMEOUT);
        switch (poolResult) {
            case -1:
                LOG << "SocketServer: error on poll()";
                return;
            case 0:LOG <<"SocketServer: timeout";
                continue;
            default:
                break;
        }
        if (m_activeConnections[0].revents & POLLIN) {
            m_activeConnections[0].revents = 0;
            --poolResult;
            accept();
         }

        for (auto it = m_activeConnections.begin() + 1; it != m_activeConnections.end() && poolResult > 0; ) {
            if (it->revents & POLLIN) {
                --poolResult;
                it->revents = 0;

                std::string message;
                ssize_t recvBytes = recvMessage(it->fd, message);

                if (recvBytes < 1) {
                    perror("error on receiving data from client");
                    m_socket->close(it->fd);
                    it = m_activeConnections.erase(it);
                    continue;
                } else {
                    std::string response;
                    if (!message.empty()) {
                        int sendBytes = m_socket->send(it->fd, message.c_str(), message.size() + 1, MSG_NOSIGNAL);
                        if (sendBytes == -1)
                           perror(" error on sending the response");
                    }
                }
            }
            it++;
        }

    }
}