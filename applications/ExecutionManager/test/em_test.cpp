#include "manifests.hpp"
#include "manifest_reader.hpp"
#include "gtest/gtest.h"
#include <iostream>
#include <string>
#include <sys/stat.h>
#include <ftw.h>
#include <unistd.h>

using namespace ExecutionManager;
using nlohmann::json;

class ManifestReaderEnvironment : public ::testing::Environment
{
public:
  ManifestReaderEnvironment();
  ~ManifestReaderEnvironment() override = default;

  void SetUp() override;

  void TearDown() override;

  const ManifestReader reader;
  const MachineManifest machineManifest;
  std::vector<std::string> nestedDirs;
  const std::string machineManifestPath;

private:
  void createMachineManifest();

  void createDirs();

};

ManifestReaderEnvironment::ManifestReaderEnvironment()
  : reader{ManifestReader{}}
  , machineManifest{MachineManifest{
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
                  }}
  , nestedDirs{std::vector<std::string>{"msm", "test-aa1", "test-aa2"}}
  , machineManifestPath{reader.corePath + "/" + nestedDirs.at(0) + "/machine_manifest.json"}
{

}

void ManifestReaderEnvironment::SetUp()
{
    createDirs();
    createMachineManifest();
}

void ManifestReaderEnvironment::TearDown()
{
    std::string rmTestDir = "rm -rf " + reader.corePath;
    system(rmTestDir.c_str());
}

void ManifestReaderEnvironment::createMachineManifest()
{
    json manifSerialized = machineManifest;

    std::ofstream output;
    output.open(machineManifestPath);
    output << manifSerialized;
}

void ManifestReaderEnvironment::createDirs()
{
    std::string createDir = "mkdir ";

    std::string createCoreDir = createDir + reader.corePath;
    system(createCoreDir.c_str());

    for (const auto& dir: nestedDirs)
    {
      std::string createNestedDir = createDir
                                  + reader.corePath
                                  + "/"
                                  + dir;

      system(createNestedDir.c_str());
    }
    system("ls -la ./test-data");
}

/// INIT ENVIRONMENT
::testing::Environment* const manifestReaderEnvironment =
    ::testing::AddGlobalTestEnvironment(new ManifestReaderEnvironment);
/// -------------------

TEST(ManifestReaderTest, getJsonDataTest)
{
  ManifestReaderEnvironment env;
  ManifestReader reader;

  json manifSerialized = env.machineManifest;

  json returnObj = reader.getJsonData(env.machineManifestPath);

  ASSERT_EQ(manifSerialized, returnObj);
}

TEST(ManifestReaderTest, getMachineStates)
{
  ManifestReader reader;

  const std::vector<MachineState> expectedOutput
      = {"Startup", "Running", "Shutdown"};
  const auto& result = reader.getMachineStates();

 ASSERT_EQ(expectedOutput, result);
}

TEST(ManifestReaderTest, getListOfApplications)
{
  ManifestReaderEnvironment env;
  ManifestReader reader;

  auto resultAppsList = reader.getListOfApplications();
  std::sort(env.nestedDirs.begin(),
            env.nestedDirs.end());

  std::sort(resultAppsList.begin(),
            resultAppsList.end());

  ASSERT_EQ(env.nestedDirs, resultAppsList);
}

//TEST_F(ManifestReaderTest, getMachineStatesTest)
//const std::map<ManifestReaderTest::TestApplicationManifestEnum,
//               ApplicationManifest>
//applicationManifests =
//{
//  {ManifestReaderTest::TestApplicationManifestEnum::TEST_AA0,
//     ApplicationManifest{
//       ApplicationManifestInternal{
//         "test-aa1",
//         {Process{
//           "proc1",
//           {ModeDepStartupConfig{
//             {MachineInstanceMode{"MachineManifest", "Startup"},
//              MachineInstanceMode{"MachineManifest", "Running"}
//             }}}
//         }}
//       }
//     }
//  },
//  {ManifestReaderTest::TestApplicationManifestEnum::TEST_AA1,
//     ApplicationManifest{
//       ApplicationManifestInternal{
//         "test-aa2",
//         {Process{
//           "proc2",
//           {ModeDepStartupConfig{
//             {MachineInstanceMode{"MachineManifest", "Startup"},
//              MachineInstanceMode{"MachineManifest", "Running"}
//             }}}
//         }}
//       }
//     }
//  },
//};
//{
//  std::vector<std::string> testResult =
//}

///**
// * @brief Check Application Manifest serialization and deserialization.
// */
//TEST(SerializationTest, applicationManifestTest)
//{

//  ApplicationManifest manifest = ApplicationManifest{
//    ApplicationManifestInternal{
//      "SomeTestApp",
//      {Process{
//        "TestApp",
//        {ModeDepStartupConfig{
//          {MachineInstanceMode{"MachineManifest", "Startup"},
//           MachineInstanceMode{"MachineManifest", "Running"}
//          }}}
//      }}
//    }
//  };

//  json manifSerialized = manifest;

//  std::string testJsonResult =
//    R"({"Application_manifest":{"Application_manifest_id":"SomeTestApp",)"
//    R"("Process":[{"Mode_dependent_startup_configs":[)"
//    R"({"Mode_in_machine_instance_refs":[{"Function_group":"MachineManifest",)"
//    R"("Mode":"Startup"},{"Function_group":"MachineManifest","Mode":"Running")"
//    R"(}]}],"Process_name":"TestApp"}]}})";

//  ApplicationManifest deserializedManifest = json::parse(testJsonResult);

//  ASSERT_EQ(manifSerialized.dump(), testJsonResult);
//  ASSERT_EQ(deserializedManifest, manifSerialized);
//}

///**
// * @brief Test Machine Manifest serialization and deserialization.
// */
//TEST(SerializationTest, machineManifestTest)
//{
//  MachineManifest manifest = MachineManifest{
//    MachineManifestInternal{
//      "Machine",
//      {
//        ModeDeclarationGroup{
//          "MachineState",
//          {
//            Mode{"Startup"},
//            Mode{"Running"},
//            Mode{"Shutdown"}
//          }
//        }
//      }
//    }
//  };

//  json manifestSerialized = manifest;

//  std::string testJsonResult =
//  R"({"Machine_manifest":{"Machine_manifest_id":"Machine")"
//  R"(,"Mode_declaration_group":[{"Function_group_name":"MachineState",)"
//  R"("Mode_declarations":[{"Mode":"Startup"},{"Mode":"Running")"
//  R"(},{"Mode":"Shutdown"}]}]}})";

//  MachineManifest deserializedManifest = json::parse(testJsonResult);

//  ASSERT_EQ(manifestSerialized.dump(), testJsonResult);
//  ASSERT_EQ(deserializedManifest, manifestSerialized);
//}
