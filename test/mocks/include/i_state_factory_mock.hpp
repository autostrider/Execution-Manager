#ifndef I_STATE_FACTORY_MOCK_HPP
#define I_STATE_FACTORY_MOCK_HPP

#include "i_state_factory.hpp"

#include "gmock/gmock.h"

namespace api
{

class StateFactoryMock : public IStateFactory
{
public:
    StateFactoryMock() = default;
    MOCK_CONST_METHOD1(createInit, std::unique_ptr<IState>(IAdaptiveApp& msm));
    MOCK_CONST_METHOD1(createRun, std::unique_ptr<IState>(IAdaptiveApp& msm));
    MOCK_CONST_METHOD1(createShutDown, std::unique_ptr<IState>(IAdaptiveApp& msm));
    MOCK_CONST_METHOD1(createSuspend, std::unique_ptr<IState>(IAdaptiveApp& msm));
};

} // namespace api

#endif // I_STATE_FACTORY_MOCK_HPP
