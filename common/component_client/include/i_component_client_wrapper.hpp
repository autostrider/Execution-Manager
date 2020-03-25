#ifndef COMPONENT_CLIENT_WRAPPER_H
#define COMPONENT_CLIENT_WRAPPER_H

#include "component_client.h"

namespace component_client {

class IComponentClientWrapper
{
public:
    virtual ComponentClientReturnType SetStateUpdateHandler
    (std::function<void(ComponentState const&)>) noexcept = 0;

    virtual ComponentClientReturnType GetComponentState
    (ComponentState&) noexcept = 0;

    virtual void ConfirmComponentState
    (ComponentState, ComponentClientReturnType) noexcept = 0;


    virtual void CheckIfAnyEventsAvailable() noexcept {};

    virtual ~IComponentClientWrapper() noexcept {}
};

class ComponentClientWrapper : public IComponentClientWrapper
{
public:
    ComponentClientWrapper(const std::string& component,
                           StateUpdateMode updateMode) noexcept;

    ~ComponentClientWrapper() noexcept;

    ComponentClientReturnType SetStateUpdateHandler
    (std::function<void(ComponentState const&)> f) noexcept override;

    ComponentClientReturnType GetComponentState
    (ComponentState& state) noexcept override;

    void CheckIfAnyEventsAvailable() noexcept override;

    void ConfirmComponentState
    (ComponentState state, ComponentClientReturnType status) noexcept override;
private:
    ComponentClient m_client;
};

} // namespace component_client

#endif // COMPONENT_CLIENT_WRAPPER_H
