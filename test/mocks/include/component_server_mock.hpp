#ifndef COMPONENT_SERVER_MOCK_HPP
#define COMPONENT_SERVER_MOCK_HPP

#include <i_component_server_wrapper.hpp>

#include "gmock/gmock.h"

namespace component_server
{

class ComponentServerMock : public IComponentServerWrapper
{
public:
    MOCK_METHOD(void, start, (), (noexcept overrade));
    MOCK_METHOD(bool, getQueueElement, (std::string&), (noexcept overrade));
    MOCK_METHOD(ComponentState, setStateUpdateHandler, (std::string), (noexcept overrade));
};

} //namespace component_server


#endif
