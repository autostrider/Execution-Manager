#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <i_client_socket.hpp>
#include <i_client.hpp>

#include <string>
#include <memory>
#include <sys/un.h>

using namespace api;

namespace base_client
{

class Client : public IClient
{
public:
    Client(const std::string&, std::unique_ptr<IClientSocket>);
    ~Client();

    void connect() override;
    
    void sendMessage(const google::protobuf::Message& context) override;

    ssize_t receive(std::string&) override;
    bool isConnected() override;
    ssize_t getRecvBytes() override;

    void setClientFd(int);
    void setConnected(bool);

    int getClientFd();

private:
    void createSocket();

private:
    std::unique_ptr<IClientSocket> m_client_socket;
    std::string m_path;
    
    bool m_connected;

    sockaddr_un m_addr;
    int m_client_fd;
    int m_addr_len;

    ssize_t m_recvBytes;
};

}

#endif //CLIENT_HPP
