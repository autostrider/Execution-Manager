#include "manifests.hpp"
#include "gtest/gtest.h"
#include <iostream>
#include <string>

using namespace ExecutionManager;
using nlohmann::json;

/**
 * @brief Check Application Manifest serialization and deserialization.
 */
// TEST(SerializationTest, applicationManifestTest)
// {

//   ApplicationManifest manifest = ApplicationManifest{
//     ApplicationManifestInternal{
//       "SomeTestApp",
//       {Process{
//         "TestApp",
//         {ModeDepStartupConfig{
//           {MachineInstanceMode{"MachineManifest", "Startup"},
//            MachineInstanceMode{"MachineManifest", "Running"}
//           }}}
//       }}
//     }
//   };

//   json manifSerialized = manifest;

//   std::string testJsonResult =
//     R"({"Application_manifest":{"Application_manifest_id":"SomeTestApp",)"
//     R"("Process":[{"Mode_dependent_startup_configs":[)"
//     R"({"Mode_in_machine_instance_refs":[{"Function_group":"MachineManifest",)"
//     R"("Mode":"Startup"},{"Function_group":"MachineManifest","Mode":"Running")"
//     R"(}]}],"Process_name":"TestApp"}]}})";

//   ApplicationManifest deserializedManifest = json::parse(testJsonResult);

//   ASSERT_EQ(manifSerialized.dump(), testJsonResult);
//   ASSERT_EQ(deserializedManifest, manifSerialized);
// }

/**
 * @brief Test Machine Manifest serialization and deserialization.
 */
TEST(SerializationTest, machineManifestTest)
{
  MachineManifest manifest = MachineManifest{
    MachineManifestInternal{
      "Machine",
      {
        ModeDeclarationGroup{
          "MachineState",
          {
            Mode{"Startup"},
            Mode{"Running"},
            Mode{"Shutdown"}
          }
        }
      }
    }
  };

  json manifestSerialized = manifest;

  std::string testJsonResult =
  R"({"Machine_manifest":{"Machine_manifest_id":"Machine")"
  R"(,"Mode_declaration_group":[{"Function_group_name":"MachineState",)"
  R"("Mode_declarations":[{"Mode":"Startup"},{"Mode":"Running")"
  R"(},{"Mode":"Shutdown"}]}]}})";

  MachineManifest deserializedManifest = json::parse(testJsonResult);

  ASSERT_EQ(manifestSerialized.dump(), testJsonResult);
  ASSERT_EQ(deserializedManifest, manifestSerialized);
}
