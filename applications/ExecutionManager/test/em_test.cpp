#include "manifests.hpp"
#include "gtest/gtest.h"
#include <iostream>
#include <string>

using namespace ExecutionManager;
using nlohmann::json;

TEST(SomeTest, DoesThis)
{
  std::string testString = "Hello from Execution Manager";
  EXPECT_EQ(testString, foo());
}

/**
 * @brief Check Application Manifest serialization and deserialization.
 */
TEST(SerializationTest, applicationManifestTest)
{

  ApplicationManifest manifest = ApplicationManifest{
    ApplicationManifestInternal{
      "SomeTestApp",
      {Process{
        "TestApp",
        {ModeDepStartupConfig{
          {MachineInstanceMode{"MachineManifest", "Startup"},
           MachineInstanceMode{"MachineManifest", "Running"}
          }}}
      }}
    }
  };

  json manifSerialized = manifest;

  std::string testJsonResult =
    "{"
      "\"Application_manifest\":{"
        "\"Application_manifest_id\":\"SomeTestApp\","
        "\"Process\":["
          "{"
            "\"Mode_dependent_startup_configs\":["
              "{"
               "\"Mode_in_machine_instance_refs\":["
                  "{"
                    "\"Function_group\":\"MachineManifest\","
                    "\"Mode\":\"Startup\""
                  "},"
                  "{"
                    "\"Function_group\":\"MachineManifest\","
                    "\"Mode\":\"Running\""
                  "}"
                "]"
              "}"
            "],"
            "\"Process_name\":\"TestApp\""
          "}"
        "]"
      "}"
    "}";

  ApplicationManifest deserializedManifest = json::parse(testJsonResult);

  ASSERT_EQ(manifSerialized.dump(), testJsonResult);
  ASSERT_EQ(deserializedManifest, manifSerialized);
}

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
  "{"
    "\"Machine_manifest\":{"
      "\"Machine_manifest_id\":\"Machine\","
      "\"Mode_declaration_group\":["
        "{"
          "\"Function_group_name\":\"MachineState\","
          "\"Mode_declarations\":["
            "{"
              "\"Mode\":\"Startup\""
            "},"
            "{"
              "\"Mode\":\"Running\""
            "},"
            "{"
              "\"Mode\":\"Shutdown\""
            "}"
          "]"
        "}"
      "]"
    "}"
  "}";

  MachineManifest deserializaedManifest = json::parse(testJsonResult);

  ASSERT_EQ(manifestSerialized.dump(), testJsonResult);
  ASSERT_EQ(deserializaedManifest, manifestSerialized);
}
