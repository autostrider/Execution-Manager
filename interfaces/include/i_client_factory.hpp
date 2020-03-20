#ifndef I_CLIENT_FACTORY_HPP
#define I_CLIENT_FACTORY_HPP

#include <memory>

namespace api::socket_handler
{

class IClient;

class IProxyClientFactory
{
public:
    virtual ~IProxyClientFactory() = default;

    virtual std::unique_ptr<IClient> makeProxyClient(const int&) const = 0;
};

}
#endif //I_CLIENT_FACTORY_HPP