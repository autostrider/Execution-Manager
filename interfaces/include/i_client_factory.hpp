#ifndef I_CLIENT_FACTORY_HPP
#define I_CLIENT_FACTORY_HPP

#include <client.hpp>

#include <memory>

class IClientFactory
{
public:
    virtual ~IClientFactory() = default;

    virtual std::shared_ptr<IClient> makeClient(const int&) const = 0;
};

#endif //I_CLIENT_FACTORY_HPP