#include <msm_state_machine.hpp>

#include <app_state_client_mock.hpp>
#include <i_state_factory_mock.hpp>
#include <machine_state_client_mock.hpp>
#include <i_machine_state_manager_mock.hpp>
#include <manifest_reader_mock.hpp>
#include <client_mock.hpp>
#include <persistent_storage_mock.hpp>

#include <any.pb.h>

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
                    std::move(clientMock),
                    std::move(storageMock)};
    }

    std::string setDataForRecv(std::string state, size_t& size);

protected:
   std::unique_ptr<AppStateClientMock> appStateClientMock =
           std::make_unique<AppStateClientMock>();
   std::unique_ptr<StateFactoryMock> factoryMock =
           std::make_unique<StateFactoryMock>();
   std::unique_ptr<MachineStateClientMock> machineStateClientMock =
           std::make_unique<MachineStateClientMock>();
   std::unique_ptr<NiceMock<ManifestReaderMock>> manifestReaderMock =
           std::make_unique<NiceMock<ManifestReaderMock>>();
   std::unique_ptr<NiceMock<ClientMock>> clientMock =
           std::make_unique<NiceMock<ClientMock>>();
   std::unique_ptr<NiceMock<KeyValueStorageMock>> storageMock =
           std::make_unique<NiceMock<KeyValueStorageMock>>();
   MsmStateFactory factory;
   IMachineStateManagerMock* msmMock;
};

std::string MsmStateMachineTest::setDataForRecv(std::string state, size_t& size)
{
    MachineStateManagement::nextState context;
    context.set_state(state);

    google::protobuf::Any message;
    message.PackFrom(context);

    size = message.ByteSizeLong();
    std::vector<uint8_t> rawData(size);
        
    message.SerializeToArray(rawData.data(), size);

    std::string data(rawData.cbegin(), rawData.cend());
    return data;
}

TEST_F(MsmStateMachineTest, ShouldInitCallEnter)
{
    EXPECT_CALL(*machineStateClientMock, Register(_,_))
        .WillOnce(Return(StateError::kSuccess));

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
    size_t size;
    size_t size1;
    size_t size2;

    std::string data = setDataForRecv("start", size);
    std::string data1 = setDataForRecv("two", size1);
    std::string data2 = setDataForRecv("Shuttingdown", size2);

    {
        EXPECT_CALL(*manifestReaderMock, getMachineStates())
            .WillOnce(Return(std::vector<std::string>{"start", "two", "Shuttingdown"}));
        EXPECT_CALL(*storageMock, GetValue("state"))
            .WillOnce(Return(ByMove(KvsType("Shuttingdown"))));
        EXPECT_CALL(*clientMock, receive(_))
            .WillOnce(
                    DoAll(
                        SetArgReferee<0>(data),
                        Return(size))
                    )
            .WillOnce(
                    DoAll(
                        SetArgReferee<0>(data1),
                        Return(size1))
                    )
            .WillOnce(
                    DoAll(
                        SetArgReferee<0>(data2),
                        Return(size2))
                    );
        EXPECT_CALL(*machineStateClientMock, SetMachineState(_,_))
            .Times(3)
            .WillRepeatedly(Return(StateError::kSuccess));
    }

    msmMock = initIMsm();

    std::unique_ptr<::Run> state = std::make_unique<::Run>(*msmMock);
    state->enter();
}

TEST_F(MsmStateMachineTest, ShouldDiscardNotExistingStatesWhenProvided)
{
    size_t size;
    size_t size1;
    size_t size2;
    size_t size3;

    std::string data = setDataForRecv("start", size);
    std::string data1 = setDataForRecv("randomState", size1);
    std::string data2 = setDataForRecv("two", size2);
    std::string data3 = setDataForRecv("Shuttingdown", size3);

    {
        EXPECT_CALL(*manifestReaderMock, getMachineStates())
            .WillOnce(Return(std::vector<std::string>{"start", "two", "Shuttingdown"}));
        EXPECT_CALL(*clientMock, receive(_))
            .WillOnce(
                DoAll(
                    SetArgReferee<0>(data),
                    Return(size))
                )
            .WillOnce(
                DoAll(
                    SetArgReferee<0>(data1),
                    Return(size1))
                )
            .WillOnce(
                DoAll(
                    SetArgReferee<0>(data2),
                    Return(size2))
                )
            .WillOnce(
                DoAll(
                    SetArgReferee<0>(data3),
                    Return(size3))
                );
        EXPECT_CALL(*machineStateClientMock, SetMachineState(_,_))
            .Times(3)
            .WillRepeatedly(Return(StateError::kSuccess));
    }
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
