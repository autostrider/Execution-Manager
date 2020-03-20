#ifndef I_CONNECTION_FACTORY_HPP
#define I_CONNECTION_FACTORY_HPP

#include <i_client_factory.hpp>

#include <memory>

class IConnection;
class IServerSocket;

class IConnectionFactory
{
public:
    virtual ~IConnectionFactory() = default;

    virtual std::shared_ptr<IConnection> makeConnection(std::shared_ptr<IServerSocket>,
                                                        int)
                                            const = 0;
};

#endif // I_CONNECTION_FACTORY_HPP