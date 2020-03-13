#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "client_socket.hpp"
#include <i_client.hpp>

#include <string>
#include <memory>
#include <sys/un.h>

class Client : public IClient
{
public:

    Client(const std::string&, std::unique_ptr<IClientSocket>);
    ~Client();

    void connect() override;
    void sendMessage(const google::protobuf::Any& message) override;
    std::string receive() override;
    bool isConnected() override;
    ssize_t getRecvBytes() override;

    void setClientFd(int);
    void setConnected(bool);
    void setRecvBytes(ssize_t byte);

    int getClientFd();

private:
    void createSocket();

private:
    std::unique_ptr<IClientSocket> m_client_socket;
    bool m_connected;
    sockaddr_un m_addr;
    int m_client_fd, m_addr_len;
    std::string m_path;

    ssize_t m_recvBytes;
};

#endif //CLIENT_HPP
