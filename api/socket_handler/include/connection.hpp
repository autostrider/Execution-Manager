#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include <proxy_client_factory.hpp>
#include <i_connection.hpp>

#include <memory>


class IClient;
class IServerSocket;


class Connection : public IConnection
{
public:
    Connection(std::shared_ptr<IServerSocket>,
               const int,
               std::shared_ptr<api::socket_handler::IProxyClientFactory>);

    ~Connection() = default;

    void creatAcceptedClient() override;
    std::string receiveData() override;

    int getRecvBytes() override;

private:
    int acceptConnection();

private:
    std::shared_ptr<api::socket_handler::IProxyClientFactory> m_clientFactory;
    std::unique_ptr<api::socket_handler::IClient> m_connectedCli;
    std::shared_ptr<IServerSocket> m_serverSocket;
    const int m_serverFd;
};

#endif // CONNECTION_HPP