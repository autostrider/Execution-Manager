#include <msm_handler.hpp>

#include "gtest/gtest.h"

using ExecutionManager::MsmHandler;

class MsmRegistrerTest : public ::testing::Test
{
protected:
  MsmHandler registrer;
  const std::string msmName{"msmName"};
  const int correctPid{2};
  const int invalidPid{3};
};

TEST_F(MsmRegistrerTest, ShouldSuccessToRegisterFirstMsm)
{
  ASSERT_TRUE(registrer.registerMsm(correctPid, msmName));
  ASSERT_EQ(correctPid, registrer.msmPid());
}

TEST_F(MsmRegistrerTest, ShouldDiscardOtherPidsWhenAlreadyRegistered)
{
  registrer.registerMsm(correctPid, msmName);

  ASSERT_FALSE(registrer.registerMsm(invalidPid, msmName));
}

TEST_F(MsmRegistrerTest, ShouldCorrectCheckForMsmPid)
{
  registrer.registerMsm(correctPid, msmName);

  ASSERT_TRUE(registrer.checkMsm(correctPid));
  ASSERT_FALSE(registrer.checkMsm(invalidPid));
}