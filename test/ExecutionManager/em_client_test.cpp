#include <execution_manager_client.hpp>

#include "gtest/gtest.h"
#include "gmock/gmock.h"

using namespace api;
using namespace ExecutionManagerClient;
using namespace testing;


class EMClientTest : public ::testing::Test
{
protected:
    std::string componentPath = "component";
    std::string msmPath = "msm";

    std::string compName = "proc1";
    std::string state = "state";
};

TEST_F(EMClientTest, s)
{
    auto io = kj::setupAsyncIo();

    std::unique_ptr<IExecutionManagerClient> emClient =
        std::make_unique<ExecutionManagerClient::ExecutionManagerClient> (msmPath, componentPath, io);

    auto res = emClient->SetComponentState(state, compName);

    EXPECT_EQ(res, pComponentClientReturnType::kSuccess);
}