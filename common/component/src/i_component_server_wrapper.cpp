#include "i_component_server_wrapper.hpp"

namespace component_server
{

ComponentServerWrapper::ComponentServerWrapper(const std::string &path,
                                               std::unique_ptr<IServerSocket> socket,
                                               std::unique_ptr<IConnectionFactory> conn) noexcept
      : m_server{path, std::move(socket), std::move(conn)}
{}

ComponentServerWrapper::~ComponentServerWrapper() noexcept
{}

std::string ComponentServerWrapper::SetStateUpdateHandler(std::string data) noexcept
{
    return m_server.setStateUpdateHandler(data);
}

} // namespace component_server
