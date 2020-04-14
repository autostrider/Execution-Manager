#include <component_client_for_wrapper.hpp>
#include <constants.hpp>
#include <logger.hpp>
#include <server.hpp>
#include <client_mock.hpp>
#include <server_socket.hpp>
#include <client_socket.hpp>
#include <connection_factory.hpp>
#include <proxy_client_factory.hpp>

#include <any.pb.h>
#include <component_state_management.pb.h>
#include <execution_management.pb.h>

#include <future>

#include "gtest/gtest.h"
#include "gmock/gmock.h"

using namespace std;
using namespace constants;
using namespace component_client;
using namespace testing;

struct TestData
{
  std::string component;
  std::string state;
  ComponentClientReturnType status;
};
  
class TestServer
{
public:
    explicit TestServer(TestData& data)
        : m_sharedResource(data)
    {
        LOG << "Server created...";
    }
    
    void start(std::string);
    void stop();
    ComponentClientReturnType stateUpdateHandler(common::ReceivedMessage&);
    ComponentClientReturnType getComponentState(std::string, ComponentClientReturnType);
    void confirmComponentState();
    void checkIfAnyEventsAvailable(std::string, int);

private:
    TestData& m_sharedResource;
    std::unique_ptr<base_server::Server> server;
    std::unique_ptr<base_server::ConnectionFactory> connection =
            std::make_unique<base_server::ConnectionFactory>(
                    std::make_unique<base_client::ProxyClientFactory>());
};

void TestServer::start(std::string path)
{
  server = std::make_unique<base_server::Server>
            (path,
             std::make_unique<socket_handler::ServerSocket>(),
             std::make_unique<base_server::ConnectionFactory>(
                    std::make_unique<base_client::ProxyClientFactory>()));

  server->start();
}

void TestServer::stop()
{
    server->stop();
}

ComponentClientReturnType TestServer::stateUpdateHandler(common::ReceivedMessage& message)
{
    bool promiseSet{false};
    
    while (message.data.empty())
    {
        server->getQueueElement(message);
    }

    google::protobuf::Any any;
    any.ParseFromString(message.data);

    if (any.Is<StateManagement::RegisterComponent>())
    {
        StateManagement::RegisterComponent context;
        any.UnpackTo(&context);
       
        m_sharedResource.component = context.component();

        context = {};
    }

    ExecutionManagement::resultRegisterComponent regResult;
    regResult.set_result(ComponentClientReturnType::kSuccess);

    server->send(regResult, message.fd);

    ExecutionManagement::setComponentState com;
    com.set_state("kOn");

    server->send(com, message.fd);
}

ComponentClientReturnType TestServer::getComponentState(std::string state, ComponentClientReturnType result)
{
    common::ReceivedMessage message;
    
    while (message.data.empty())
    {
        server->getQueueElement(message);
    }

    google::protobuf::Any any;
    any.ParseFromString(message.data);
      
    if (any.Is<StateManagement::GetComponentState>())
    {
        StateManagement::GetComponentState context;
        any.UnpackTo(&context);
       
        m_sharedResource.component = context.component();
       
        context = {};
    }

    ExecutionManagement::resultGetComponentState context;
    context.set_state(state);
    context.set_result(result);

    server->send(context, message.fd);
}

void TestServer::confirmComponentState()
{
    common::ReceivedMessage message;
    
    while (message.data.empty())
    {
        server->getQueueElement(message);
    }

    google::protobuf::Any any;
    any.ParseFromString(message.data);

    if (any.Is<StateManagement::ConfirmComponentState>())
    {
        StateManagement::ConfirmComponentState context;
        any.UnpackTo(&context);
       
        m_sharedResource.component = context.component();
        m_sharedResource.state = context.state();
        m_sharedResource.status = context.status();
       
        context = {};
    }
}

void TestServer::checkIfAnyEventsAvailable(std::string state, int fd)
{
    ExecutionManagement::setComponentState context;
    context.set_state(state);

    server->send(context, fd);
}

class ComponentClientTest : public Test 
{
protected:
    ~ComponentClientTest() noexcept(true) override {}

    ComponentClientTest() {}

    void SetUp() override
    {
        ::unlink(EM_SOCKET_NAME.c_str());
    }

    void TearDown() override
    {
        ::unlink(EM_SOCKET_NAME.c_str());
    }

    void expectCheckIfAnyEventsAvailable();
    ComponentClientReturnType expectSetStateUpdateHandler();

    const uint32_t defaultTimeout{666};

    TestData testData;
    const std::string socketName{EM_SOCKET_NAME};

    std::unique_ptr<ClientMock> clientMock = std::make_unique<ClientMock>();
    ClientMock *clientMockPtr = clientMock.get();

    std::unique_ptr<Client> client = std::make_unique<Client>(EM_SOCKET_NAME, std::make_unique<socket_handler::ClientSocket>());

    TestServer testServer{testData};

    component_client::StateUpdateMode mode = component_client::StateUpdateMode::kPoll;
    component_client::StateUpdateMode eventMode = component_client::StateUpdateMode::kEvent;
    std::string componentName = "TestName";
    std::string eventComponentName = "ComponentWithEventMode";

    ComponentClientForWrapper cc {componentName, mode};
    ComponentClientForWrapper ccEventMode {eventComponentName, eventMode};
};


ComponentClientReturnType ComponentClientTest::expectSetStateUpdateHandler()
{
  std::function<void(std::string const&)> f = [](std::string const& res){};

  return ccEventMode.setStateUpdateHandler(f);
}

void ComponentClientTest::expectCheckIfAnyEventsAvailable()
{
  ccEventMode.checkIfAnyEventsAvailable();
}

TEST_F(ComponentClientTest, ShouldSucceedToSetStateUpdateHandlerUsingClientMock)
{
    {
        EXPECT_CALL(*clientMockPtr, connect()).WillOnce(Return());
        EXPECT_CALL(*clientMockPtr, sendMessage(_));
        EXPECT_CALL(*clientMockPtr, receive(_)).WillOnce(Return(8));
    }
    ccEventMode.setClient(std::move(clientMock));

    expectSetStateUpdateHandler();
}

TEST_F(ComponentClientTest, ShouldSucceedToSetStateUpdateHandler)
{
    common::ReceivedMessage message;

    testServer.start(socketName);

    auto tread = std::thread( [&] () {
        
        ccEventMode.setClient(std::move(client));
        auto result = expectSetStateUpdateHandler();

        EXPECT_EQ(result, ComponentClientReturnType::kSuccess);
    });

    testServer.stateUpdateHandler(message);

    EXPECT_EQ(testData.component, eventComponentName);

    tread.join();
    testServer.stop();
}

TEST_F(ComponentClientTest, ShouldEnterCheckIfAnyEventsAvailable)
{
    testData.state = "TestComponentState";
    common::ReceivedMessage message;

    testServer.start(socketName);

    auto tread = std::thread( [&] () {

        ccEventMode.setClient(std::move(client));

        auto result = expectSetStateUpdateHandler();
        expectCheckIfAnyEventsAvailable();

        EXPECT_EQ(result, ComponentClientReturnType::kSuccess);
    });

    testServer.stateUpdateHandler(message);
    testServer.checkIfAnyEventsAvailable(testData.state, message.fd);

    tread.join();
    testServer.stop();
}

TEST_F(ComponentClientTest, ShouldSucceedToGetComponentClientState)
{
    std::string setState = "TestComponentState";
    std::string state;

    testServer.start(socketName);

    auto tread = std::thread( [&] () {
        
        ccEventMode.setClient(std::move(client));
        const auto result = ccEventMode.GetComponentState(state);

        EXPECT_EQ(result, ComponentClientReturnType::kSuccess);
        EXPECT_EQ(setState, state);
    });

    testServer.getComponentState(setState, ComponentClientReturnType::kSuccess);

    tread.join();
    testServer.stop();
}

TEST_F(ComponentClientTest, ShouldSucceedToConfirmComponentState)
{
    std::string state = "TestComponentState";
    ComponentClientReturnType status = ComponentClientReturnType::kSuccess;

    testServer.start(socketName);

    auto tread = std::thread( [&] () {
        cc.setClient(std::move(client));
        cc.ConfirmComponentState(state, status);
    });

    testServer.confirmComponentState();

    EXPECT_EQ(testData.component, componentName);
    EXPECT_EQ(testData.state, state);
    EXPECT_EQ(testData.status, status);

    tread.join();
    testServer.stop();
}