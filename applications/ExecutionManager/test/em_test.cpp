#include "manifests.hpp"
#include "manifest_reader.hpp"
#include "gtest/gtest.h"
#include <iostream>
#include <string>
#include <sys/stat.h>
#include <unistd.h>

using namespace ExecutionManager;
using nlohmann::json;


class ManifestReaderTest : public ::testing::Test
{
protected:
  void SetUp() override;
  void TearDown() override;
protected:
  const ManifestReaderConf testConf{"./test-data/",
                                    "./test-data/msm/machine_manifest.json"};

private:
  void createMachineManifest();
  void createDirs();
  void createApplicationManifests();
};

void ManifestReaderTest::SetUp()
{
  createDirs();
  createMachineManifest();
  createApplicationManifests();
}

void ManifestReaderTest::TearDown()
{
    std::string rmTestDir = "rm -rf " + testConf.corePath;
    system(rmTestDir.c_str());
}

void ManifestReaderTest::createDirs()
{
    std::string createDir = "mkdir ";
    std::vector<std::string> nestedDirs =
      {"msm", "test-aa1", "test-aa2"};

    std::string createCoreDir = createDir +testConf.corePath;
    system(createCoreDir.c_str());

    for (const auto& dir: nestedDirs)
    {
      std::string createNestedDir = createDir
                                    + testConf.corePath
                                    + dir;

      system(createNestedDir.c_str());
    }
}

void ManifestReaderTest::createMachineManifest()
{
  const MachineManifest machineManifest =
    MachineManifest{
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

  json manifSerialized = machineManifest;

    std::ofstream output{testConf.machineManifestPath};
    output << manifSerialized;
}

void ManifestReaderTest::createApplicationManifests()
{
  const std::map<std::string, ApplicationManifest> applicationManifestsInfo =
    {
    {"test-aa1",
       ApplicationManifest{
         ApplicationManifestInternal{
           "test-aa1",
           {Process{
             "proc1",
             {ModeDepStartupConfig{
               {MachineInstanceMode{"MachineState", "Running"}
               }}}
           }}
         }
       }
      },
      {"test-aa2",
       ApplicationManifest{
         ApplicationManifestInternal{
           "test-aa2",
           {Process{
             "proc2",
             {ModeDepStartupConfig{
               {MachineInstanceMode{"MachineState", "Startup"}
               }}}
           }}
         }
       }
      },
      {"msm",
       ApplicationManifest{
         ApplicationManifestInternal{
           "msm",
           {Process{
             "msm",
             {ModeDepStartupConfig{
               {MachineInstanceMode{"MachineState", "Startup"},
                MachineInstanceMode{"MachineState", "Running"}
               }}}
           }}
         }
       }
      }
  };
  for (const auto& appManifest: applicationManifestsInfo)
  {
    std::string path = testConf.corePath
                       + appManifest.first
                       + "/manifest.json";
    std::ofstream output{path};

    json applicationManifestForPath = appManifest.second;
    output << applicationManifestForPath;
  }
}

TEST_F(ManifestReaderTest, ShouldReturnAvailableMachineStates)
{
  ManifestReader reader{testConf};

  auto res = reader.getMachineStates();

  ASSERT_EQ(res,
            (std::vector<MachineState>{"Startup", "Running", "Shutdown"}));
}

TEST_F(ManifestReaderTest, ShouldFaildWhenDirectoryNotExists)
{
  ManifestReader reader{ManifestReaderConf{"./not-exists", "manifest"}};

  EXPECT_THROW(reader.getStatesSupportedByApplication(), std::runtime_error);
}

TEST_F(ManifestReaderTest, ShouldOtherKeysThanMachineStateWhenParse)
{
  std::string redundantMachineManifest =
      R"({"Machine_manifest":{"Machine_manifest_id":"Machine",)"
      R"("Mode_declaration_group":[{"Function_group_name":"MachineState",)"
      R"("Mode_declarations":[{"Mode":"Startup"},{"Mode":"Running"},)"
      R"({"Mode":"Shutdown"}]},{"Function_group_name":"TestGroupName",)"
      R"("Mode_declarations":[{"Mode":"tes1"},{"Mode":"tes2"}]}]}})";

  std::ofstream testOutput{testConf.machineManifestPath};
  testOutput << redundantMachineManifest;
  testOutput.close();

  ManifestReader reader{testConf};
  auto res = reader.getMachineStates();

  ASSERT_EQ(res,
            (std::vector<MachineState>{"Startup", "Running", "Shutdown"}));

}

TEST_F(ManifestReaderTest, ShouldEmptyReturnWhenNoMachineStateFunctionGroup)
{
  std::string redundantMachineManifest =
      R"({"Machine_manifest":{"Machine_manifest_id":"Machine",)"
      R"("Mode_declaration_group":[{"Function_group_name":"TestGroupName",)"
      R"("Mode_declarations":[{"Mode":"tes1"},{"Mode":"tes2"}]}]}})";

  ManifestReaderConf conf{"./test-data", "./test-data/msm/test-conf.json"};

  std::ofstream testOutput{conf.machineManifestPath};
  testOutput << redundantMachineManifest;
  testOutput.close();


  ManifestReader reader{conf};
  auto res = reader.getMachineStates();
  ASSERT_EQ(res,
            (std::vector<MachineState>{}));

}


TEST_F(ManifestReaderTest, ShouldEmptyReturnWhenEmptyMachineStates)
{
  std::string redundantMachineManifest =
      R"({"Machine_manifest":{"Machine_manifest_id":"Machine",)"
      R"("Mode_declaration_group":[]}})";

  ManifestReaderConf conf{"./test-data", "./test-data/msm/test-conf.json"};

  std::ofstream testOutput{conf.machineManifestPath};
  testOutput << redundantMachineManifest;
  testOutput.close();


  ManifestReader reader{conf};
  auto res = reader.getMachineStates();
  ASSERT_EQ(res,
            (std::vector<MachineState>{}));

}


TEST_F(ManifestReaderTest, ShouldReturnMapOfApplicationsForStates)
{
  ManifestReader reader{testConf};
  std::map<MachineState, std::vector<ProcessName>> expectedResult =
  {
      {"Startup", {
                   ProcessName{"msm", "msm"},
                   ProcessName{"test-aa2", "proc2"}
                   }},
      {"Running", {
                   ProcessName{"test-aa1", "proc1"},
                   ProcessName{"msm", "msm"}
                  }},
  };

  auto result = reader.getStatesSupportedByApplication();
  for (auto& appsForState: result)
  {
    ASSERT_EQ(appsForState.second.size(),
              expectedResult[appsForState.first].size());

    const auto& expResultForState = expectedResult[appsForState.first];

    for (size_t i = 0; i < expResultForState.size(); i++)
    {
      ASSERT_EQ(appsForState.second.at(i).processName,
                expResultForState.at(i).processName);
      ASSERT_EQ(appsForState.second.at(i).applicationName,
                expResultForState.at(i).applicationName);
    }
  }
}

TEST_F(ManifestReaderTest, ShouldDiscardRedundantFunctionGroupsWhenProvided)
{
  std::string input =
      R"({"Application_manifest":{"Application_manifest_id":)"
      R"("app","Process":[{"Mode_dependent_startup_configs":)"
      R"([{"Mode_in_machine_instance_refs":[{"Function_group":"MachineState",)"
      R"("Mode": "Startup"},{"Function_group": "MachineState","Mode":)"
      R"("Running"},{"Function_group": "Test", "Mode": "test"}]}],"Process_name":)"
      R"("app"}]}})";
  system("mkdir ./test-data/app");

  std::ofstream testOutput{"./test-data/app/manifest.json"};
  testOutput << input;
  testOutput.close();

  ManifestReader reader{testConf};
  std::map<MachineState, std::vector<ProcessName>> expectedResult =
  {
      {"Startup", {
                   ProcessName{"app", "app"},
                   ProcessName{"msm", "msm"},
                   ProcessName{"test-aa2", "proc2"}
                   }},
      {"Running", {
                   ProcessName{"app", "app"},
                   ProcessName{"test-aa1", "proc1"},
                   ProcessName{"msm", "msm"}
                  }},
  };


  auto result = reader.getStatesSupportedByApplication();

  for (auto& appsForState: result)
  {
    ASSERT_EQ(appsForState.second.size(),
              expectedResult[appsForState.first].size());

    const auto& expResultForState = expectedResult[appsForState.first];

    for (size_t i = 0; i < expResultForState.size(); i++)
    {
      ASSERT_EQ(appsForState.second.at(i).processName,
                expResultForState.at(i).processName);
      ASSERT_EQ(appsForState.second.at(i).applicationName,
                expResultForState.at(i).applicationName);
    }
  }
}

TEST_F(ManifestReaderTest, ShouldEmptyMapWhenNoProvided)
{
  ManifestReaderConf conf{"./empty-manifest/", ""};

  system(("mkdir -p " + conf.corePath + "/app").c_str());

  std::string input =
      R"({"Application_manifest":{"Application_manifest_id":)"
      R"("app","Process":[]}})";

  std::ofstream testOutput{conf.corePath + "/app/manifest.json"};
  testOutput << input;
  testOutput.close();

  ManifestReader reader{conf};
  auto result = reader.getStatesSupportedByApplication();

  ASSERT_EQ(result,
            (std::map<MachineState, std::vector<ProcessName>>{}));

  system(("rm -rf " + conf.corePath).c_str());
}
