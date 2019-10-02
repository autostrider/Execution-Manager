#include "msm_registrer.hpp"

#include "gtest/gtest.h"

using ExecutionManager::MsmRegister;

class MsmRegistrerTest : public ::testing::Test
{
protected:
  MsmRegister registrer;
  const std::string msmName{"msmName"};
  const int correctPid{2};
  const int failPid{3};
};

TEST_F(MsmRegistrerTest, ShouldSuccessToRegisterFirstMsm)
{
  bool result = registrer.registerMsm(correctPid, msmName);

  ASSERT_TRUE(result);
  ASSERT_EQ(correctPid, registrer.msmPid());
}

TEST_F(MsmRegistrerTest, ShouldDiscardOtherPidsWhenAlreadyRegistered)
{
  registrer.registerMsm(correctPid, msmName);
  bool result = registrer.registerMsm(failPid, msmName);

  ASSERT_FALSE(result);
}

TEST_F(MsmRegistrerTest, ShouldCorrectCheckForMsmPid)
{
  registrer.registerMsm(correctPid, msmName);

  ASSERT_TRUE(registrer.checkMsm(correctPid));
  ASSERT_FALSE(registrer.checkMsm(failPid));
}