#ifndef I_STATE_MOCK_HPP
#define I_STATE_MOCK_HPP

#include "i_state.hpp"

#include "gmock/gmock.h"

namespace api
{

class IStateMock : public IState
{
public:
    MOCK_METHOD(void, enter, ());
    MOCK_METHOD(void, leave, (), (const));
};

} // namespace api

#endif // I_STATE_MOCK_HPP