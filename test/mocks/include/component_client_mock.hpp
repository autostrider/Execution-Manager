#ifndef COMPONENT_CLIENT_MOCK_HPP
#define COMPONENT_CLIENT_MOCK_HPP

#include <i_component_client_wrapper.hpp>

#include "gmock/gmock.h"

namespace api
{

class ComponentClientMock : public api::IComponentClientWrapper
{
public:
    MOCK_METHOD1(SetStateUpdateHandler, api::ComponentClientReturnType(std::function<void(api::ComponentState const&)> f));

    MOCK_METHOD1(GetComponentState, api::ComponentClientReturnType(api::ComponentState& state));

    MOCK_METHOD2(ConfirmComponentState, void(api::ComponentState state, 
                                             api::ComponentClientReturnType status));

    MOCK_METHOD0(CheckIfAnyEventsAvailable, void());


/*
    MOCK_METHOD(api::ComponentClientReturnType, SetStateUpdateHandler,
                (std::function<void(api::ComponentState const&)> f), (noexcept));

    MOCK_METHOD(api::ComponentClientReturnType, GetComponentState,
                (api::ComponentState& state), (noexcept));

    MOCK_METHOD(void, ConfirmComponentState,
                (api::ComponentState state, api::ComponentClientReturnType status), (noexcept));

    MOCK_METHOD(void, CheckIfAnyEventsAvailable, (), (noexcept));

*/
};

} //namespace api


#endif
