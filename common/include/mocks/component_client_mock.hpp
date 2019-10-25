#ifndef COMPONENT_CLIENT_MOCK_HPP
#define COMPONENT_CLIENT_MOCK_HPP

#include <i_component_client_wrapper.hpp>
#include "gmock/gmock.h"

namespace api
{

class ComponentClientMock : public api::IComponentClientWrapper
{
public:
    MOCK_METHOD(api::ComponentClientReturnType, SetStateUpdateHandler,
                (std::function<void(api::ComponentState const&)> f), (noexcept));

    MOCK_METHOD(api::ComponentClientReturnType, GetComponentState,
                (api::ComponentState& state), (noexcept));

    MOCK_METHOD(void, ConfirmComponentState,
                (api::ComponentState state, api::ComponentClientReturnType status), (noexcept));
};

} //namespace api


#endif
