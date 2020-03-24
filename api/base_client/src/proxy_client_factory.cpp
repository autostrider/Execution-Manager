#include "proxy_client_factory.hpp"
#include <constants.hpp>

namespace api::client
{

std::unique_ptr<IClient> ProxyClientFactory::makeProxyClient(const int& fd) const
{
    auto client = std::make_unique<Client>(SOCKET_CLIENT_PATH, std::make_unique<ClientSocket>());
    
    client->setClientFd(fd);
    client->setConnected(true);
    return client;
}

}