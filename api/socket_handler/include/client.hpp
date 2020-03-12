#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "client_socket.hpp"

#include <string>
#include <memory>
#include <sys/un.h>

class Client {
public:

    Client(const std::string&, std::unique_ptr<IClientSocket>);

    ~Client();

    void connect();

    template < typename T >
    void sendMessage(const T& message)
    {
        size_t size = message.ByteSizeLong();
        uint8_t* data = new uint8_t[size];
        if(message.SerializeToArray(data, size))
        {
            m_client_socket->send(m_client_fd, data, size, 0);
        }
        delete[] data;
    }

    std::string receive(bool&);

    bool isConnected();

    void setClientFd(int);
    void setConnected(bool);
    void setRecvBytes(ssize_t byte);

    int getClientFd();
    ssize_t getRecvBytes();

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
