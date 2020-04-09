#include <mscForWrapper.hpp>
#include <constants.hpp>
#include <server.hpp>
#include <server_socket.hpp>
#include <client_socket.hpp>
#include <connection_factory.hpp>
#include <proxy_client_factory.hpp>
#include <client_mock.hpp>

#include <any.pb.h>

#include <thread>

#include "gtest/gtest.h"

using machine_state_client::MachineStateClient;
using namespace machine_state_client;
using namespace testing;


class MachineStateClientTest
: public ::testing::Test
{
public:
  ~MachineStateClientTest() noexcept
  {}

protected:
  //MachineStateManagement::registerApp setRegisterData();

protected:
  const std::string applicationName{"TestMSC"};
  const std::string path{"/tmp/testMsc"};
  const uint32_t defaultTimeout{666};
  
  google::protobuf::Any message;
  
  std::unique_ptr<ClientMock> clientMock = std::make_unique<ClientMock>();
  ClientMock *clientMockPtr = clientMock.get();
};

// MachineStateManagement::registerApp MachineStateClientTest::setRegisterData()
// {
//   MachineStateManagement::registerApp context;
//   context.set_appname(applicationName);
//   context.set_pid(getpid());

//   return context;
// }


TEST_F(MachineStateClientTest, ShouldSuccessfullyRegister)
{
  auto expextResult = machine_state_client::StateError::kSuccess;
  std::string data;
  //MachineStateManagement::registerApp registerContext = setRegisterData();

  MachineStateManagement::resultRegisterApp context;
  context.set_result(expextResult);

  message.PackFrom(context);
  message.SerializeToString(&data);

  {
    EXPECT_CALL(*clientMockPtr, connect()).WillOnce(Return());
    EXPECT_CALL(*clientMockPtr, sendMessage(_));
    EXPECT_CALL(*clientMockPtr, receive(_)).WillOnce(
                DoAll(
                    SetArgReferee<0>(data),
                    Return(data.size()))
                );
  }

  MscForWrapper msc{path};
  msc.setClient(std::move(clientMock));

  const auto result = msc.Register(applicationName, defaultTimeout);

  EXPECT_EQ(result, expextResult);
}

TEST_F(MachineStateClientTest, ShouldGetTimeoutOnRegister)
{
  {
    EXPECT_CALL(*clientMockPtr, connect()).WillOnce(Return());
    EXPECT_CALL(*clientMockPtr, sendMessage(_));
    EXPECT_CALL(*clientMockPtr, receive(_)).WillRepeatedly(Return(0));
  }

  MscForWrapper msc{path};
  msc.setClient(std::move(clientMock));

  const auto result = msc.Register(applicationName, defaultTimeout);

  EXPECT_EQ(result, machine_state_client::StateError::kTimeout);
}


TEST_F(MachineStateClientTest, ShouldSucceedToGetMachineState)
{
  auto expextResult = machine_state_client::StateError::kSuccess;
  std::string state{"Running"};
  std::string data;

  MachineStateManagement::resultGetMachineState context;
  context.set_result(expextResult);

  message.PackFrom(context);
  message.SerializeToString(&data);

  {
    EXPECT_CALL(*clientMockPtr, connect()).WillRepeatedly(Return());
    EXPECT_CALL(*clientMockPtr, sendMessage(_));
    EXPECT_CALL(*clientMockPtr, receive(_)).WillOnce(
                DoAll(
                    SetArgReferee<0>(data),
                    Return(data.size()))
                );
  }

  MscForWrapper msc{path};
  msc.setClient(std::move(clientMock));

  const auto result = msc.GetMachineState(state, defaultTimeout);

  EXPECT_EQ(result, expextResult);
}

TEST_F(MachineStateClientTest, ShouldTimeoutOnGetMachineState)
{
  std::string state{"Running"};
  {
    EXPECT_CALL(*clientMockPtr, connect()).WillOnce(Return());
    EXPECT_CALL(*clientMockPtr, sendMessage(_));
    EXPECT_CALL(*clientMockPtr, receive(_)).WillRepeatedly(Return(0));
  }

  MscForWrapper msc{path};
  msc.setClient(std::move(clientMock));

  const auto result = msc.GetMachineState(state, defaultTimeout);

  EXPECT_EQ(result, machine_state_client::StateError::kTimeout);
}

TEST_F(MachineStateClientTest, ShouldSucceedToSetMachineState)
{
  auto expextResult = machine_state_client::StateError::kSuccess;
  std::string state{"Running"};
  std::string data;

  MachineStateManagement::resultSetMachineState context;
  context.set_result(expextResult);

  message.PackFrom(context);
  message.SerializeToString(&data);

  {
    EXPECT_CALL(*clientMockPtr, connect()).WillRepeatedly(Return());
    EXPECT_CALL(*clientMockPtr, sendMessage(_));
    EXPECT_CALL(*clientMockPtr, receive(_)).WillOnce(
                DoAll(
                    SetArgReferee<0>(data),
                    Return(data.size()))
                );
  }

  MscForWrapper msc{path};
  msc.setClient(std::move(clientMock));

  const auto result = msc.SetMachineState(state, defaultTimeout);

  EXPECT_EQ(result, expextResult);
}

TEST_F(MachineStateClientTest, ShouldTimeoutOnSetMachineState)
{
  std::string state{"Running"};
  {
    EXPECT_CALL(*clientMockPtr, connect()).WillOnce(Return());
    EXPECT_CALL(*clientMockPtr, sendMessage(_));
    EXPECT_CALL(*clientMockPtr, receive(_)).WillRepeatedly(Return(0));
  }

  MscForWrapper msc{path};
  msc.setClient(std::move(clientMock));

  const auto result = msc.SetMachineState(state, defaultTimeout);

  EXPECT_EQ(result, machine_state_client::StateError::kTimeout);
}