#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "client_socket.hpp"

#include <string>
#include <memory>
#include <sys/un.h>

class Client {
public:

    Client(const std::string &path, std::unique_ptr<IClientSocket> socket);

    ~Client();

    bool isConnected();

    void setConnected(bool connected);

    int getClientFd();

    void setClientFd(int fd);

    void sendBytes(std::string &message) const;

    void connect();

    ssize_t receive(std::string &message) const;

private:
    void createSocket();

private:
    std::unique_ptr<IClientSocket> m_client_socket;
    bool m_connected;
    sockaddr_un m_addr;
    int m_client_fd, m_addr_len;
    std::string m_path;
};

#endif //CLIENT_HPP
