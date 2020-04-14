#ifndef COMPONENT_CLIENT_FOR_WRAPPER_HPP
#define COMPONENT_CLIENT_FOR_WRAPPER_HPP

#include "component_client.h"

namespace component_client
{

class ComponentClientForWrapper : public ComponentClient
{
public:
    ComponentClientForWrapper(const std::string &componentName, StateUpdateMode mode);
    void setClient(std::unique_ptr<IClient> client);
};

}

#endif // COMPONENT_CLIENT_FOR_WRAPPER_HPP