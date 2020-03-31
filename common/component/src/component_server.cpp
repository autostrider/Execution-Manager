#include "component_server.hpp"

#include <any.pb.h>

namespace component_server
{

ComponentServer::ComponentServer(const std::string &path,
                                 std::unique_ptr<IServerSocket> socket,
                                 std::unique_ptr<IConnectionFactory> conn)
    : Server{path, std::move(socket), std::move(conn)}
{}

std::string ComponentServer::setStateUpdateHandler(std::string recv) noexcept
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