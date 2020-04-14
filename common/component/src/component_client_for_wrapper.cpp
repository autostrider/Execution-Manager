#include "component_client_for_wrapper.hpp"

namespace component_client
{

ComponentClientForWrapper::ComponentClientForWrapper(const std::string &componentName, StateUpdateMode mode)
    : ComponentClient(componentName, mode)
{}

void ComponentClientForWrapper::setClient(std::unique_ptr<IClient> client)
{
    ComponentClient::setClient(std::move(client));
}

}