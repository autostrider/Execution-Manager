#ifndef COMPONENT_SERVER_WRAPPER_HPP
#define COMPONENT_SERVER_WRAPPER_HPP

#include "component_server.hpp"

namespace component_server
{

class IComponentServerWrapper
{
public:
    virtual ~IComponentServerWrapper() noexcept {}

    virtual std::string SetStateUpdateHandler(std::string data) noexcept = 0;
};

class ComponentServerWrapper : public IComponentServerWrapper
{
public:
    ComponentServerWrapper(const std::string &path,
                           std::unique_ptr<IServerSocket> socket,
                           std::unique_ptr<IConnectionFactory> conn) noexcept;

    ~ComponentServerWrapper() noexcept;

    std::string SetStateUpdateHandler(std::string data) noexcept override;

private:
    ComponentServer m_server;
};

} // namespace component_server

#endif // COMPONENT_SERVER_WRAPPER_HPP
