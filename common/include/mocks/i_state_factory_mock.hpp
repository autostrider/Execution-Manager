#ifndef I_STATE_FACTORY_MOCK_HPP
#define I_STATE_FACTORY_MOCK_HPP

#include "i_state_factory.hpp"

#include "gmock/gmock.h"

class IStateFactoryMock : public api::IStateFactory
{
public:
    IStateFactoryMock() = default;
    MOCK_METHOD(std::unique_ptr<api::IState>, createInit, (api::IAdaptiveApp& msm), (const));
    MOCK_METHOD(std::unique_ptr<api::IState>, createRun, (api::IAdaptiveApp& msm), (const));
    MOCK_METHOD(std::unique_ptr<api::IState>, createShutDown, (api::IAdaptiveApp& msm), (const));
};

#endif // I_STATE_FACTORY_MOCK_HPP