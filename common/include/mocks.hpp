#ifndef MOCKS_HPP
#define MOCKS_HPP

#include "machine_state_manager.hpp"

#include "gmock/gmock.h"

using namespace MSM;

using StateError = api::MachineStateClient::StateError;

class IMachineStateManagerMock : public MachineStateManager
{
    public:
        IMachineStateManagerMock(std::unique_ptr<api::IStateFactory> factory,
                                 std::unique_ptr<api::IApplicationStateClientWrapper> appClient,
                                 std::unique_ptr<api::IMachineStateClientWrapper> machineClient);

        MOCK_METHOD(void, init, ());
        MOCK_METHOD(void, run, ());
        MOCK_METHOD(void, terminate, ());

        MOCK_METHOD(void, reportApplicationState, (api::ApplicationStateClient::ApplicationState state));
        MOCK_METHOD(void, transitToNextState, (IMachineStateManagerMock::FactoryFunc nextState));
};

class IStateFactoryMock : public api::IStateFactory
{
public:
    IStateFactoryMock() = default;
    MOCK_METHOD(std::unique_ptr<api::IState>, createInit, (api::IAdaptiveApp& msm), (const));
    MOCK_METHOD(std::unique_ptr<api::IState>, createRun, (api::IAdaptiveApp& msm), (const));
    MOCK_METHOD(std::unique_ptr<api::IState>, createShutDown, (api::IAdaptiveApp& msm), (const));
};

class IStateMock : public api::IState
{
public:
    IStateMock() = default;
    MOCK_METHOD(void, enter, ());
    MOCK_METHOD(void, leave, (), (const));
};

class AppStateClientMock : public api::IApplicationStateClientWrapper
{
public:
    AppStateClientMock() = default;
    MOCK_METHOD(void, ReportApplicationState, (ApplicationStateManagement::ApplicationState state));
};

class MachineStateClientMock : public api::IMachineStateClientWrapper
{
public:
    MachineStateClientMock() = default;
    MOCK_METHOD(StateError, Register, (std::string appName, std::uint32_t timeout));
    MOCK_METHOD(StateError, GetMachineState, (std::uint32_t timeout, std::string& state));
    MOCK_METHOD(StateError, SetMachineState, (std::string state, std::uint32_t timeout));
    MOCK_METHOD(StateError, waitForConfirm, (std::uint32_t timeout));
};

#endif