#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include <proxy_client_factory.hpp>
#include <i_connection.hpp>
#include <i_server_socket.hpp>
#include <i_client.hpp>

#include <memory>

namespace base_server
{

using namespace api;

class Connection : public IConnection
{
public:
    Connection(std::shared_ptr<IServerSocket>,
               const int,
               std::shared_ptr<IProxyClientFactory>);

    void creatAcceptedClient() override;
    int receiveData(std::string&) override;

    int getRecvBytes() override;
    void sendData(const google::protobuf::Message& context) override;
    int getFd() override;

private:
    int acceptConnection();

private:
    std::shared_ptr<IProxyClientFactory> m_clientFactory;
    std::unique_ptr<IClient> m_connectedCli;
    std::shared_ptr<IServerSocket> m_serverSocket;
    const int m_serverFd;
};

}

#endif // CONNECTION_HPP
