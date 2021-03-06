#include <application_state_client.h>
#include <constants.hpp>
#include <logger.hpp>

#include <iostream>
#include <unistd.h>

#include <capnp/ez-rpc.h>
#include <execution_management.capnp.h>

#include "gtest/gtest.h"
#include "gmock/gmock.h"

using namespace ::testing;
using namespace constants;

namespace ApplicationStateClientTest
{

using ApplicationState = ::ApplicationStateManagement::ApplicationState;

struct Data
{
    ApplicationState m_state;
    pid_t m_appPid;
};

class ApplicationStateClientServer : public ::ExecutionManagement::Server
{
public:
    explicit
    ApplicationStateClientServer (Data& sharedResource);

private:
    ::kj::Promise<void>
    reportApplicationState(ReportApplicationStateContext context) override;

private:
    Data& m_sharedResource;
};

ApplicationStateClientServer::ApplicationStateClientServer(Data& sharedResource) 
    : m_sharedResource{sharedResource}
{
    LOG << "Application State Client server started...";
}

::kj::Promise<void>
ApplicationStateClientServer::reportApplicationState(ReportApplicationStateContext context)
{
    m_sharedResource.m_state = context.getParams().getState();

    return kj::READY_NOW;
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
    capnp::EzRpcServer server
    {kj::heap<ApplicationStateClientServer>(sharedResource), socketName}; 
};

TEST_F(ApplicationStateClientTest, ShouldSucceedToReportApplicationState)
{
	application_state::ApplicationStateClient asc;
	asc.ReportApplicationState(ApplicationState::K_RUNNING);

	ASSERT_EQ(ApplicationState::K_RUNNING, sharedResource.m_state);
}

} //namespace
