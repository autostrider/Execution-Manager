#ifndef COMPONENT_CLIENT_WRAPPER_H
#define COMPONENT_CLIENT_WRAPPER_H

#include "component_client_for_wrapper.hpp"

namespace component_client
{

class IComponentClientWrapper
{
public:
    virtual ~IComponentClientWrapper() noexcept(false) {}

    virtual void setClient(std::unique_ptr<IClient> client) = 0;
    virtual ComponentClientReturnType GetComponentState(ComponentState&) noexcept = 0;
    virtual ComponentClientReturnType setStateUpdateHandler(std::function<void(ComponentState const&)> f) noexcept = 0;
    virtual void ConfirmComponentState(ComponentState,
                                       ComponentClientReturnType) noexcept = 0;
    virtual void checkIfAnyEventsAvailable() = 0;
};

class ComponentClientWrapper : public IComponentClientWrapper
{
public:
    ComponentClientWrapper(const std::string& component,
                           StateUpdateMode updateMode) noexcept;

    void setClient(std::unique_ptr<IClient> client) override;
    ComponentClientReturnType GetComponentState(ComponentState& state) noexcept override;
    ComponentClientReturnType setStateUpdateHandler(std::function<void(ComponentState const&)> f) noexcept override;
    void ConfirmComponentState(ComponentState state,
                               ComponentClientReturnType status) noexcept override;
    void checkIfAnyEventsAvailable() override;

private:
    ComponentClientForWrapper m_client;
};

} // namespace component_client

#endif // COMPONENT_CLIENT_WRAPPER_H
