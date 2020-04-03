#include <msm_state_machine.hpp>

#include <app_state_client_mock.hpp>
#include <i_state_factory_mock.hpp>
#include <machine_state_client_mock.hpp>
#include <i_machine_state_manager_mock.hpp>
#include <manifest_reader_mock.hpp>
#include <socket_server_mock.hpp>
#include <persistent_storage_mock.hpp>

#include "gtest/gtest.h"

using namespace api;
using namespace application_state;
using namespace machine_state_client;
using namespace MSM;
using namespace testing;

class MsmStateMachineTest : public ::testing::Test
{
protected:
    void TearDown() override
    {
        delete msmMock;
    }

    IMachineStateManagerMock* initIMsm()
    {
        return new IMachineStateManagerMock{std::move(factoryMock),
                    std::move(appStateClientMock),
                    std::move(machineStateClientMock),
                    std::move(manifestReaderMock),
                    std::move(socketServerMock),
                    std::move(storageMock)};
    }

   std::unique_ptr<AppStateClientMock> appStateClientMock =
           std::make_unique<AppStateClientMock>();
   std::unique_ptr<StateFactoryMock> factoryMock =
           std::make_unique<StateFactoryMock>();
   std::unique_ptr<MachineStateClientMock> machineStateClientMock =
           std::make_unique<MachineStateClientMock>();
   std::unique_ptr<NiceMock<ManifestReaderMock>> manifestReaderMock =
           std::make_unique<NiceMock<ManifestReaderMock>>();
   std::unique_ptr<NiceMock<SocketServerMock>> socketServerMock =
           std::make_unique<NiceMock<SocketServerMock>>();
   std::unique_ptr<NiceMock<KeyValueStorageMock>> storageMock =
           std::make_unique<NiceMock<KeyValueStorageMock>>();
   MsmStateFactory factory;
   IMachineStateManagerMock* msmMock;
};

TEST_F(MsmStateMachineTest, ShouldInitCallEnter)
{
    EXPECT_CALL(*machineStateClientMock, Register(_,_))
        .WillOnce(Return(StateError::K_SUCCESS));

    msmMock = initIMsm();

    EXPECT_CALL(*msmMock, reportApplicationState(_)).WillOnce(Return());

    std::unique_ptr<Init> state = std::make_unique<Init>(*msmMock);
    state->enter();
}

TEST_F(MsmStateMachineTest, UnsuccessfulRegistration)
{
    EXPECT_CALL(*machineStateClientMock, Register(_,_))
        .WillOnce(Return(StateError::kInvalidState));

    msmMock = initIMsm();

    EXPECT_CALL(*msmMock, reportApplicationState(_)).WillOnce(Return());

    std::unique_ptr<Init> state = std::make_unique<Init>(*msmMock);
    state->enter();
}

TEST_F(MsmStateMachineTest, ShouldRunCallEnter)
{
    EXPECT_CALL(*manifestReaderMock, getMachineStates())
      .WillOnce(Return(std::vector<std::string>{"start", "two", "Shuttingdown"}));
    EXPECT_CALL(*storageMock, GetValue("state"))
      .WillOnce(Return(ByMove(KvsType("Shuttingdown"))));
    EXPECT_CALL(*socketServerMock, getData())
      .WillOnce(Return("start"))
      .WillOnce(Return("two"))
      .WillOnce(Return("Shuttingdown"));
    EXPECT_CALL(*machineStateClientMock, SetMachineState(_,_))
        .Times(3)
        .WillRepeatedly(Return(StateError::K_SUCCESS));

    msmMock = initIMsm();

    std::unique_ptr<::Run> state = std::make_unique<::Run>(*msmMock);
    state->enter();
}

TEST_F(MsmStateMachineTest, ShouldDiscardNotExistingStatesWhenProvided)
{
  EXPECT_CALL(*manifestReaderMock, getMachineStates())
          .WillOnce(Return(std::vector<std::string>{"start", "two", "Shuttingdown"}));
  EXPECT_CALL(*socketServerMock, getData())
          .WillOnce(Return("start"))
          .WillOnce(Return("randomState"))
          .WillOnce(Return("two"))
          .WillOnce(Return("Shuttingdown"));
  EXPECT_CALL(*machineStateClientMock, SetMachineState(_,_))
          .Times(3)
          .WillRepeatedly(Return(StateError::K_SUCCESS));

  msmMock = initIMsm();

  std::unique_ptr<::Run> state = std::make_unique<::Run>(*msmMock);
  state->enter();
}

TEST_F(MsmStateMachineTest, ShouldInitCallLeave)
{
    msmMock = initIMsm();

    EXPECT_CALL(*msmMock, reportApplicationState(_)).WillOnce(Return());

    std::unique_ptr<Init> state = std::make_unique<Init>(*msmMock);
    state->leave();
}

TEST_F(MsmStateMachineTest, ShouldTerminateCallEnter)
{
    msmMock = initIMsm();

    EXPECT_CALL(*msmMock, reportApplicationState(_)).WillOnce(Return());

    std::unique_ptr<ShutDown> state = std::make_unique<ShutDown>(*msmMock);
    state->enter();
}

TEST_F(MsmStateMachineTest, ShouldTerminateCallLeave)
{
    msmMock = initIMsm();

    std::unique_ptr<ShutDown> state = std::make_unique<ShutDown>(*msmMock);
    state->leave();
}

TEST_F(MsmStateMachineTest, ShouldCreateInit)
{
    msmMock = initIMsm();

    std::unique_ptr<IState> expectedState = std::make_unique<Init>(*msmMock);
    std::unique_ptr<IState> createdState = factory.createInit(*msmMock);

    bool result = std::is_same<decltype (expectedState), decltype (createdState)>::value;
    ASSERT_TRUE(result);
}

TEST_F(MsmStateMachineTest, ShouldCreateRun)
{
    msmMock = initIMsm();

    std::unique_ptr<IState> expectedState = std::make_unique<::Run>(*msmMock);
    std::unique_ptr<IState> createdState = factory.createRun(*msmMock);

    bool result = std::is_same<decltype (expectedState), decltype (createdState)>::value;
    ASSERT_TRUE(result);
}

TEST_F(MsmStateMachineTest, ShouldCreateTerminate)
{
    msmMock = initIMsm();

    std::unique_ptr<IState> expectedState = std::make_unique<ShutDown>(*msmMock);
    std::unique_ptr<IState> createdState = factory.createShutDown(*msmMock);

    bool result = std::is_same<decltype (expectedState), decltype (createdState)>::value;
    ASSERT_TRUE(result);
}
