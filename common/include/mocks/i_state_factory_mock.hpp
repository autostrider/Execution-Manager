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
    MOCK_METHOD(std::unique_ptr<IState>, createInit, (IAdaptiveApp& msm), (const));
    MOCK_METHOD(std::unique_ptr<IState>, createRun, (IAdaptiveApp& msm), (const));
    MOCK_METHOD(std::unique_ptr<IState>, createShutDown, (IAdaptiveApp& msm), (const));
};

} // namespace api

#endif // I_STATE_FACTORY_MOCK_HPP
