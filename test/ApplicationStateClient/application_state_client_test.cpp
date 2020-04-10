#include <asc_for_wrapper.hpp>
#include <constants.hpp>
#include <logger.hpp>
#include <server.hpp>
#include <client_mock.hpp>
#include <server_socket.hpp>
#include <client_socket.hpp>
#include <connection_factory.hpp>
#include <proxy_client_factory.hpp>

#include <any.pb.h>
#include <application_state_management.pb.h>

#include "gtest/gtest.h"
#include "gmock/gmock.h"

using namespace ::testing;
using namespace constants;

namespace ApplicationStateClientTest
{

using ApplicationState = application_state::ApplicationStateClient::ApplicationState;

struct Data
{
    ApplicationState m_state;
    pid_t m_appPid;
};

class TestServer
{
public:
    explicit TestServer(Data& sharedResource);
    
    void start(std::string);
    void stop();
    void reportApplicationState();

private:
    Data& m_sharedResource;
    std::unique_ptr<base_server::Server> server;
    std::unique_ptr<base_server::ConnectionFactory> connection = std::make_unique<base_server::ConnectionFactory>(
                    std::make_unique<base_client::ProxyClientFactory>());
};

TestServer::TestServer(Data& sharedResource) 
    : m_sharedResource{sharedResource}
{
    LOG << "Application State Client server started...";
}

void TestServer::reportApplicationState()
{
    std::string data;
    
    while (data.empty())
    {
        server->getQueueElement(data);
    }


    google::protobuf::Any any;
    any.ParseFromString(data);
      
    if (any.Is<ApplicationStateManagement::ReportApplicationState>())
    {
        ApplicationStateManagement::ReportApplicationState context;
        any.UnpackTo(&context);
       
        m_sharedResource.m_state = context.state();
       
        context = {};
    }
}

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

class ApplicationStateClientTest : public Test 
{
protected:
    ~ApplicationStateClientTest() noexcept(true) override {}

    ApplicationStateClientTest() {}

    void SetUp() override
    {
        ::unlink(EM_SOCKET_NAME.c_str());
    }

    void TearDown() override
    {
        ::unlink(EM_SOCKET_NAME.c_str());
    }

    Data sharedResource;
    const std::string socketName{IPC_PROTOCOL + EM_SOCKET_NAME};

    std::unique_ptr<ClientMock> clientMock = std::make_unique<ClientMock>();
    ClientMock *clientMockPtr = clientMock.get();

    std::unique_ptr<Client> client = std::make_unique<Client>(EM_SOCKET_NAME, std::make_unique<socket_handler::ClientSocket>());

	application_state::AscForWrapper asc;
    TestServer testServer{sharedResource};
};

TEST_F(ApplicationStateClientTest, ShouldSucceedSendReportApplicationState)
{
    {
        EXPECT_CALL(*clientMockPtr, connect()).WillOnce(Return());
        EXPECT_CALL(*clientMockPtr, sendMessage(_));
    }
    asc.setClient(std::move(clientMock));
	asc.ReportApplicationState(ApplicationState::kRunning);
}

TEST_F(ApplicationStateClientTest, ShouldSucceedToReportApplicationState)
{
    testServer.start(EM_SOCKET_NAME);
    asc.setClient(std::move(client));

	asc.ReportApplicationState(ApplicationState::kRunning);

    testServer.reportApplicationState();

	ASSERT_EQ(ApplicationState::kRunning, sharedResource.m_state);

    testServer.stop();
}

} //namespace
