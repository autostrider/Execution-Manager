#ifndef I_STATE_MOCK_HPP
#define I_STATE_MOCK_HPP

#include "i_state.hpp"

#include "gmock/gmock.h"

namespace api
{

class StateMock : public IState
{
public:
    MOCK_METHOD0(enter, void());
    MOCK_CONST_METHOD0(leave, void();
    MOCK_METHOD0(performAction, void());
};

} // namespace api

#endif // I_STATE_MOCK_HPP
