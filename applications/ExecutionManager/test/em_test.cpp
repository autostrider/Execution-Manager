#include "manifests.hpp"
#include "gtest/gtest.h"
#include <iostream>
#include <string>

TEST(manifest_structures, json_serialization)
{
  ExecutionManager::MachineManifest manif;
  manif.init();

  ASSERT_EQ(manif.hwConf.cpu, 96);
}
