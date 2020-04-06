#ifndef COMPONENT_SERVER_WRAPPER_HPP
#define COMPONENT_SERVER_WRAPPER_HPP

#include "component_server.hpp"

namespace component_server
{

class IComponentServerWrapper 
{
public:
    virtual ~IComponentServerWrapper() noexcept {}

    virtual void start() noexcept = 0;
    virtual bool getQueueElement(std::string&) noexcept = 0;
    virtual ComponentState setStateUpdateHandler(std::string) noexcept = 0;

};

class ComponentServerWrapper : public IComponentServerWrapper
{
public:
    ComponentServerWrapper(const std::string &component,
                           std::unique_ptr<IServerSocket> socket,
                           std::unique_ptr<IConnectionFactory> conn) noexcept;

    ~ComponentServerWrapper() noexcept;

    void start() noexcept override;
    bool getQueueElement(std::string& data) noexcept override;
    ComponentState setStateUpdateHandler(std::string data) noexcept override;

private:
    ComponentServer m_server;
};

} // namespace component_server

#endif // COMPONENT_SERVER_WRAPPER_HPP
