#ifndef COMPONENT_CLIENT_WRAPPER_H
#define COMPONENT_CLIENT_WRAPPER_H

#include "component_client.h"

namespace component_client
{

class IComponentClientWrapper
{
public:
    virtual ~IComponentClientWrapper() noexcept(false) {}

    virtual ComponentClientReturnType GetComponentState(ComponentState&) noexcept = 0;
    virtual ComponentState setStateUpdateHandler() noexcept = 0;
    virtual void ConfirmComponentState(ComponentState,
                                       ComponentClientReturnType) noexcept = 0;
};

class ComponentClientWrapper : public IComponentClientWrapper
{
public:
    ComponentClientWrapper(const std::string& component,
                           StateUpdateMode updateMode) noexcept;

    ComponentClientReturnType GetComponentState(ComponentState& state) noexcept override;
    ComponentState setStateUpdateHandler() noexcept override;
    void ConfirmComponentState(ComponentState state,
                               ComponentClientReturnType status) noexcept override;

private:
    ComponentClient m_client;
};

} // namespace component_client

#endif // COMPONENT_CLIENT_WRAPPER_H
