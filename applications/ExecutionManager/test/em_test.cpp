#include "manifests.hpp"
#include "gtest/gtest.h"
#include <iostream>
#include <string>

TEST(SomeTest, DoesThis)
{
  std::string testString = "Hello from Execution Manager";
  EXPECT_EQ(testString, foo());
}
