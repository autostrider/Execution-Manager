#include "component_server.hpp"
#include <logger.hpp>

#include <any.pb.h>

namespace component_server
{

ComponentServer::ComponentServer(const std::string &component,
                                 std::unique_ptr<IServerSocket> socket,
                                 std::unique_ptr<IConnectionFactory> conn)
    : Server{component, std::move(socket), std::move(conn)}
{}

ComponentState ComponentServer::setStateUpdateHandler(std::string recv) noexcept
{
    google::protobuf::Any any;

    any.ParseFromString(recv);

    if(any.Is<pExecutionManagement::pSetCompState>())
    {
        pExecutionManagement::pSetCompState comp;
        any.UnpackTo(&comp);
        return comp.state();
    }
}

}