#ifndef I_STATE_MOCK_HPP
#define I_STATE_MOCK_HPP

#include <i_state.hpp>

#include "gmock/gmock.h"

namespace api
{

class StateMock : public IState
{
public:
    MOCK_METHOD(void, enter,         (), (override));
    MOCK_METHOD(void, leave,         (), (const, override));
    MOCK_METHOD(void, performAction, (), (override));
};

} // namespace api

#endif // I_STATE_MOCK_HPP
