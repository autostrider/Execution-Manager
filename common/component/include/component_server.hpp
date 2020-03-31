#ifndef COMPONENT_SERVER_HPP
#define COMPONENT_SERVER_HPP

#include <server.hpp>
#include <enums.pb.h>
#include <execution_management_p.pb.h>

using namespace base_server;

namespace component_server
{
using ComponentState = std::string;

class ComponentServer : public Server
{
public:
    ComponentServer(const std::string&, std::unique_ptr<IServerSocket>, std::unique_ptr<IConnectionFactory>);

    std::string setStateUpdateHandler(std::string recv) noexcept;
};

}

#endif // COMPONENT_SERVER_HPP