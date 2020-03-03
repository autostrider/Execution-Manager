#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include <string>
#include <memory>
#include "client_connection.hpp"
#include <sys/un.h>

class Connection {
public:

    Connection(const std::string &path, std::unique_ptr<IConnection> socket);

    ~Connection();

    bool isConnected();

    void setConnected(bool connected);

    int getConnectionFd();

    void setConnectionFd(int fd);

    void sendBytes(std::string &message) const;

    void createSocket();

    void connect();

    ssize_t receive(std::string &message) const;

private:
    std::unique_ptr<IConnection> m_client_socket;
    bool m_connected;
    sockaddr_un m_addr;
    int m_client_fd, m_addr_len;
    std::string m_path;
};

#endif //CONNECTION_HPP
