#ifndef COMPONENT_CLIENT_MOCK_HPP
#define COMPONENT_CLIENT_MOCK_HPP

#include <i_component_client_wrapper.hpp>

#include "gmock/gmock.h"

namespace component_client
{

class ComponentClientMock : public IComponentClientWrapper
{
public:
    MOCK_METHOD(void, setClient, (std::unique_ptr<IClient> client));
    MOCK_METHOD(ComponentClientReturnType, GetComponentState,
                (ComponentState& state), (noexcept));

    MOCK_METHOD(void, ConfirmComponentState,
                (ComponentState state, ComponentClientReturnType status), (noexcept));

    MOCK_METHOD(ComponentClientReturnType, setStateUpdateHandler, (std::function<void(ComponentState const&)> f), (noexcept));
    MOCK_METHOD(void, checkIfAnyEventsAvailable, ());
};

} //namespace component_client


#endif
