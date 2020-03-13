#include "client_factory.hpp"
#include <client.hpp>
#include <constants.hpp>

std::shared_ptr<IClient> ClientFactory::makeClient(const int& fd) const
{
    auto client = std::make_shared<Client>(SOCKET_CLIENT_PATH, std::make_unique<ClientSocket>());
    
    client->setClientFd(fd);
    client->setConnected(true);
    return client;
}