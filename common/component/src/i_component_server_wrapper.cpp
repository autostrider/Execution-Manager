#include "i_component_server_wrapper.hpp"

namespace component_server
{

ComponentServerWrapper::ComponentServerWrapper(const std::string &component,
                                               std::unique_ptr<IServerSocket> socket,
                                               std::unique_ptr<IConnectionFactory> conn) noexcept
      : m_server{component, std::move(socket), std::move(conn)}
{}

ComponentServerWrapper::~ComponentServerWrapper() noexcept
{}

void ComponentServerWrapper::start() noexcept
{
    m_server.start();
}

bool ComponentServerWrapper::getQueueElement(std::string& data) noexcept
{
    m_server.getQueueElement(data);
}

ComponentState ComponentServerWrapper::setStateUpdateHandler(std::string data) noexcept
{
    return m_server.setStateUpdateHandler(data);
}

} // namespace component_server
