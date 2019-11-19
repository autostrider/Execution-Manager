#include "common.hpp"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

TEST(ParseServiceNameTest, ShouldSuccessfulyParseServiceName)
{
  static const std::string correctPath = "/path/to/Application/processes/Process";

  ASSERT_EQ("Application_Process", parseServiceName(correctPath));
}

TEST(ParseServiceNameTest, ShouldFailWhenIncorrectPathProvided)
{
  static const std::string incorrectPath = "/some/path/application/process";

  ASSERT_NE(parseServiceName(incorrectPath), "application_process");
}