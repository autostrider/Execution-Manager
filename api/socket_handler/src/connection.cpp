#include "connection.hpp"
#include <constants.hpp>
#include <logger.hpp>

    Connection::Connection(const std::string &path, std::unique_ptr<IConnection> socket)
            : m_client_socket{std::move(socket)},
              m_path{path},
              m_connected{false},
              m_addr{} {
        m_addr.sun_family = AF_UNIX;
        m_path.copy(m_addr.sun_path, m_path.size() + 1);
        m_addr_len = (sizeof(m_addr));
    }


    void Connection::createSocket() {
        m_client_fd = m_client_socket->socket(AF_UNIX, SOCK_STREAM, 0);
        if (m_client_fd < 0) {
            LOG << "Error opening socket\n";
        }
    }

    void Connection::connect() {

        if (m_client_socket->connect(m_client_fd, (const struct sockaddr *) &m_addr, m_addr_len) != -1) {
            m_connected = true;
        } else {
            LOG << "Client failed to connect socket\n";
        }
    }

    Connection::~Connection() {
        m_client_socket->close(m_client_fd);
    }

    void Connection::sendBytes(std::string &message) const {

        ssize_t sendBytes = m_client_socket->send(m_client_fd, message.c_str(), message.size() + 1, MSG_NOSIGNAL);

        if (sendBytes == -1)
            LOG << " The connection is closed by the client\n";
    }

    ssize_t Connection::receive(std::string &message) const {
        char buffer[RECV_BUFFER_SIZE];
        ssize_t recvBytes = m_client_socket->recv(m_client_fd, buffer, RECV_BUFFER_SIZE - 1, 0);
        if (recvBytes == -1) {
            return -1;
        }
        buffer[recvBytes] = 0;
        message.append(buffer);
        std::cout << message;
        return recvBytes;
    }

    int Connection::getConnectionFd() {
        return m_client_fd;
    }

    void Connection::setConnectionFd(int fd) {
        m_client_fd = fd;
    }

    void Connection::setConnected(bool connected) {
        m_connected = connected;
    }

    bool Connection::isConnected() {
        return m_connected;
    }

