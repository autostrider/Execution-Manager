#ifndef ISTATEMOCK
#define ISTATEMOCK

#include "i_state.hpp"

#include "gmock/gmock.h"

class IStateMock : public api::IState
{
public:
    IStateMock() = default;
    MOCK_METHOD(void, enter, ());
    MOCK_METHOD(void, leave, (), (const));
};

#endif