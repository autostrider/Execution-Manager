# include "server.hpp"
#include <logger.hpp>
#include <fcntl.h>
#include <mutex>
#include <constants.hpp>

    Server::Server(const std::string &path, std::unique_ptr<IServerSocket> socket)
            : m_server_socket{std::move(socket)},
              m_server_fd{m_server_socket->socket(AF_UNIX, SOCK_STREAM, 0)},
              m_isRunning{false},
              m_addr{},
              m_path{path},
              m_activeConnections{},
              m_serverThread{},
              m_receiveThread{} {

        if (m_server_fd == -1) {

            LOG << " ServerSocket: Failed to create socket\n";
        } else {
            m_addr.sun_family = AF_UNIX;
            m_path.copy(m_addr.sun_path, m_path.size() + 1);
            m_addr_len = sizeof(m_addr);

            bind();
            listen();
        }
    }

    void Server::bind() {
        if (m_server_socket->bind(m_server_fd, (struct sockaddr *) &m_addr, m_addr_len) == -1)
            LOG << "SocketServer: error on bind()\n";
    }

    void Server::listen() {
        if (m_server_socket->listen(m_server_fd, 1) == -1)
            LOG << "SocketServer: error on listen()\n";
    }

    Server::~Server() {
    stop();
        if (m_server_socket->close(m_server_fd) == -1)
            LOG << "SocketServer: error on close()\n";
        if (m_server_socket->remove(m_path.c_str()) == -1)
            LOG << "SocketServer: error on remove()\n";

        ::unlink(m_addr.sun_path);
    }

    int Server::accept() {

        int client_fd = m_server_socket->accept(m_server_fd, (struct sockaddr *) &m_addr, (socklen_t *) &m_addr_len);

        if (client_fd < 0) {
            LOG << "SocketServer: there is no clients connected\n";
            return -1;
        }

        if (fcntl(client_fd, F_SETFL, O_NONBLOCK) == -1) {
            return -1;
        }
        return client_fd;
    }


    bool Server::start() {

        if (m_isRunning)
            return false;
        m_isRunning = true;
        m_serverThread = std::thread(&Server::onRunning, this);
        m_receiveThread = std::thread(&Server::handleConnections, this);

        return true;
    }


    void Server::stop() {
        m_isRunning = false;
        if (m_serverThread.joinable())
            m_serverThread.join();
        if (m_receiveThread.joinable())
            m_receiveThread.join();
    }

    void Server::handleConnections() {
        while (m_isRunning) {
            for (int i = 0; i < m_activeConnections.size(); ++i) {

                std::string message;

                auto recvBytes = m_activeConnections[i]->receive(message);

                if (recvBytes < 1) {
                    if (recvBytes == 0) {
                        LOG << "recv(): connection is closed by the client\n";

                        m_server_socket->close(m_activeConnections[i]->getClientFd());
                        std::lock_guard<std::mutex> guard(m_mtx2);
                        m_activeConnections.erase(m_activeConnections.begin() + i);
                        --i;
                        continue;
                    } else if (recvBytes == -1) {
                        if (!message.empty()) {
                            LOG << "Error on receiving  data\n";
                            return;
                        } else
                            continue;
                    }
                }
                if (!message.empty()) {
                    m_activeConnections[i]->sendBytes(message);
                } else
                    continue;
            }
        }
    }


    void Server::onRunning() {
        m_activeConnections.reserve(NUMBER_RESERVED_CONNECTIONS);

        while (m_isRunning) {
            int result = accept();

            if (result > 0) {
                auto client = std::make_unique<Client>(SOCKET_SERVER_PATH, std::move(std::make_unique<ClientSocket>()));
                client->setClientFd(result);
                client->setConnected(true);
                std::lock_guard<std::mutex> guard(m_mtx1);
                m_activeConnections.push_back(std::move(client));
            } else {
                continue;
            }
        }
    }

