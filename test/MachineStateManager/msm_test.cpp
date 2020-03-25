#include <machine_state_manager.hpp>
#include <logger.hpp>

#include <app_state_client_mock.hpp>
#include <i_state_factory_mock.hpp>
#include <i_state_mock.hpp>
#include <machine_state_client_mock.hpp>
#include <manifest_reader_mock.hpp>
#include <persistent_storage_mock.hpp>
#include <socket_server_mock.hpp>

#include "gtest/gtest.h"

using namespace api;
using namespace application_state;
using namespace machine_state_client;
using namespace MSM;
using namespace testing;

class MsmTest : public ::testing::Test
{
protected:
   std::unique_ptr<StateFactoryMock> factoryMock =
           std::make_unique<StateFactoryMock>();
   std::unique_ptr<AppStateClientMock> appStateClientMock =
           std::make_unique<AppStateClientMock>();
   std::unique_ptr<MachineStateClientMock> machineStateClientMock =
           std::make_unique<MachineStateClientMock>();
   std::unique_ptr<ManifestReaderMock> manifestReaderMock =
           std::make_unique<NiceMock<ManifestReaderMock>>();
   std::unique_ptr<StrictMock<KeyValueStorageMock>> storageMock =
           std::make_unique<StrictMock<KeyValueStorageMock>>();

   std::unique_ptr<NiceMock<StateMock>> stateInitMock =
           std::make_unique<NiceMock<StateMock>>();
   std::unique_ptr<NiceMock<StateMock>> stateRunMock =
           std::make_unique<NiceMock<StateMock>>();
   std::unique_ptr<NiceMock<StateMock>> stateTermMock =
           std::make_unique<NiceMock<StateMock>>();
   std::unique_ptr<StrictMock<SocketServerMock>> sockServerMock =
           std::make_unique<StrictMock<SocketServerMock>>();

   StateFactoryMock* pFactoryMock = factoryMock.get();

   void SetUp() override
   {
       EXPECT_CALL(*pFactoryMock, createInit((_)))
           .WillOnce(Return(ByMove(std::move(stateInitMock))));
   }

    MachineStateManager initMsm()
    {
        return MachineStateManager(
                std::move(factoryMock),
                std::move(appStateClientMock),
                std::move(machineStateClientMock),
                std::move(manifestReaderMock),
                std::move(sockServerMock),
                std::move(storageMock));
    }
};

class ReportingStateTest : public MsmTest
{
protected:
    void SetUp() override
    {
        appStateClientMock = std::make_unique<AppStateClientMock>();
    }
};

TEST_F(MsmTest, ShouldTransitToInitState)
{
    auto msm = initMsm();
     msm.init();
}

TEST_F(MsmTest, ShouldTransitToRunState)
{
    EXPECT_CALL(*factoryMock,
                createRun((_)))
            .WillOnce(Return(ByMove(std::move(stateRunMock))));

     auto msm = initMsm();

     msm.init();
     msm.run();
}

TEST_F(MsmTest, ShouldTransitToTerminateState)
{
    EXPECT_CALL(*factoryMock,
                createShutDown((_)))
            .WillOnce(Return(ByMove(std::move(stateTermMock))));

     auto msm = initMsm();

     msm.init();
     msm.terminate();
}

TEST_F(ReportingStateTest, ShouldReportCurrentState)
{
    EXPECT_CALL(*appStateClientMock, ReportApplicationState(_)).WillOnce(Return());

    auto msm = initMsm();

    msm.reportApplicationState(ApplicationStateClient::ApplicationState::K_RUNNING);
}
