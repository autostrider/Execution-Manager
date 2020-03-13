#ifndef CLIENT_FACTORY_HPP
#define CLIENT_FACTORY_HPP

#include <i_client_factory.hpp>

class ClientFactory : public IClientFactory
{
public:
    std::shared_ptr<IClient> makeClient(const int&) const override;
};

#endif // CLIENT_FACTORY_HPP