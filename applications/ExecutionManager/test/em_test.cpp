#include "em_in.hpp"
#include "manifests.hpp"
#include "gtest/gtest.h"
#include <iostream>
#include <string>

TEST(SomeTest, DoesThis)
{
  std::string testString = "Hello from Execution Manager";
  EXPECT_EQ(testString, foo());
}

TEST(manifest_structures, json_serialization)
{
  // test only non-trivial algorithms
  // it is only available cpu power
  manifests::MachineManifest manif;
  manif.init();

  ASSERT_EQ(manif.hwConf.cpu, 96);
  
}
