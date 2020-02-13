#ifndef EXECUTION_MANAGER_CLIENT_HPP
#define EXECUTION_MANAGER_CLIENT_HPP

#include <string>
#include <memory>
#include "i_socket.hpp"
#include "client_socket.hpp"
#include <sys/un.h>

class Client {
public:
    Client(const std::string &path, std::unique_ptr<ClientSocket> socket);

    ~Client();

    bool isConnected();
    void setConnected(bool connected);
    int getClientFd();
    void setClientFd(int fd);
    void sendBytes(std::string &message) const;
    void createSocket();
    void connect();

    ssize_t receive(std::string &message) const;

private:
    std::unique_ptr<ClientSocket> m_client_socket;
    bool m_connected;
    struct sockaddr_un m_addr;
    int m_client_fd, m_addr_len;
    std::string m_path;
};

#endif //EXECUTION_MANAGER_CLIENT_HPP
