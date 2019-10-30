#include "application_state_client.h"
#include <constants.hpp>
#include <iostream>
#include <unistd.h>
#include <logger.hpp>

#include <capnp/ez-rpc.h>
#include <execution_management.capnp.h>

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
		unlink(EM_SOCKET_NAME.c_str());
	}

	virtual void TearDown()
    {
        unlink(EM_SOCKET_NAME.c_str());
    }

    Data sharedResource;  
    const std::string socketName{IPC_PROTOCOL + EM_SOCKET_NAME};
    capnp::EzRpcServer server{kj::heap<ApplicationStateClientServer>(sharedResource), socketName}; 
};

TEST_F(ApplicationStateClientTest, ShouldSucceedToReportApplicationState)
{
    LOG << "Test Start";
	api::ApplicationStateClient asc;
    LOG << "ApplicationStateClient";
	asc.ReportApplicationState(ApplicationState::K_RUNNING);
    LOG << "ReportApplicationState";

	ASSERT_EQ(ApplicationState::K_RUNNING, sharedResource.m_state);
    LOG << "Test DONE";
}
} //namespace