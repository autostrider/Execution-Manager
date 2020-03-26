#ifndef I_CONNECTION_FACTORY_HPP
#define I_CONNECTION_FACTORY_HPP

#include <i_server_socket.hpp>

#include <memory>

namespace api
{

class IConnection;

class IConnectionFactory
{
public:
    virtual ~IConnectionFactory() = default;

    virtual std::shared_ptr<IConnection> makeConnection(std::shared_ptr<IServerSocket>,
                                                        int) const = 0;
};

}

#endif // I_CONNECTION_FACTORY_HPP