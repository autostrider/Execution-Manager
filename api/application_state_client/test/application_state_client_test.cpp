#include "application_state_client.h"

#include <capnp/ez-rpc.h>
#include <execution_management.capnp.h>
#include <iostream>
#include <unistd.h>

#include "gtest/gtest.h"
#include "gmock/gmock.h"

using namespace std;

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

ApplicationStateClientServer::ApplicationStateClientServer (Data& sharedResource) : m_sharedResource{sharedResource}
{
    cout << "Application State Client server started..." << endl;
}

::kj::Promise<void>
ApplicationStateClientServer::reportApplicationState(ReportApplicationStateContext context)
{
    m_sharedResource.m_state = context.getParams().getState();
    m_sharedResource.m_appPid = context.getParams().getPid();
         
    return kj::READY_NOW;
}

class ApplicationStateClientTest : public ::testing::Test 
{
protected:
    virtual ~ApplicationStateClientTest() noexcept(true) {}

    ApplicationStateClientTest() {}

	virtual void SetUp()
	{
		::unlink(socketName);
	}

	virtual void TearDown() {}

    Data sharedResource;  
    const char* socketName = "/tmp/execution_management"; 
    capnp::EzRpcServer server{kj::heap<ApplicationStateClientServer>(sharedResource), string{"unix:"} + socketName}; 
};

TEST_F(ApplicationStateClientTest, ShouldSucceedToReportApplicationState)
{
  api::ApplicationStateClient asc;
  asc.ReportApplicationState(ApplicationState::K_RUNNING);

  ASSERT_EQ(ApplicationState::K_RUNNING, sharedResource.m_state);
}
} //namespace
