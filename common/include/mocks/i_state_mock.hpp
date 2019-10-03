#ifndef I_STATE_MOCK_HPP
#define I_STATE_MOCK_HPP

#include "i_state.hpp"

#include "gmock/gmock.h"

class IStateMock : public api::IState
{
public:
    IStateMock() = default;
    MOCK_METHOD(void, enter, ());
    MOCK_METHOD(void, leave, (), (const));
};

#endif // I_STATE_MOCK_HPP