#include "manifests.hpp"
#include "manifest_reader.hpp"
#include "gtest/gtest.h"
#include <iostream>
#include <string>
#include <sys/stat.h>
#include <unistd.h>

using namespace ExecutionManager;
using nlohmann::json;


class ManifestReaderTests : public ::testing::Test
{
protected:
  void SetUp() override;
  void TearDown() override;
protected:
  const ManifestReaderConf testConf{"./test-data/",
                                    "./test-data/msm/machine_manifest.json"};
  const std::vector<MachineState> emptyMachineStates{};

  void writeJson(const std::string& content, const std::string path);
private:
  void createMachineManifest();
  void createDirs();
  void createApplicationManifests();
};

void ManifestReaderTests::SetUp()
{
  createDirs();
  createMachineManifest();
  createApplicationManifests();
}

void ManifestReaderTests::TearDown()
{
    std::string rmTestDir = "rm -rf " + testConf.corePathToApplicationsFolder;
    system(rmTestDir.c_str());
}

void ManifestReaderTests::writeJson(const std::string &content, const std::string path)
{
  std::ofstream output{path};
  output << content;
}

void ManifestReaderTests::createDirs()
{
    std::string createDir = "mkdir ";
    std::vector<std::string> nestedDirs =
      {"msm", "test-aa1", "test-aa2"};

    std::string createCoreDir = createDir +testConf.corePathToApplicationsFolder;
    system(createCoreDir.c_str());

    for (const auto& dir: nestedDirs)
    {
      std::string createNestedDir = createDir
                                    + testConf.corePathToApplicationsFolder
                                    + dir;

      system(createNestedDir.c_str());
    }
}

void ManifestReaderTests::createMachineManifest()
{
  const std::string machineManifest =
      R"({"Machine_manifest":{"Machine_manifest_id":"Machine",)"
      R"("Mode_declaration_group":[{"Function_group_name":"MachineState",)"
      R"("Mode_declarations":[{"Mode": "Startup"},{"Mode": "Running"},)"
      R"({"Mode": "Shutdown"}]}]}})";

  writeJson(machineManifest, testConf.machineManifestPath);
}

void ManifestReaderTests::createApplicationManifests()
{
  const std::map<std::string, std::string> applicationManifestsInfo =
    {
    {"test-aa1",
       R"({"Application_manifest":{"Application_manifest_id":"test-aa1",)"
       R"("Process":[{"Mode_dependent_startup_configs":)"
       R"([{"Mode_in_machine_instance_refs":)"
       R"([{"Function_group":"MachineState","Mode":"Running"}]}],)"
       R"("Process_name":"proc1"}]}})"
      },
      {"test-aa2",
       R"({"Application_manifest":{"Application_manifest_id":"test-aa2",)"
       R"("Process":[{"Mode_dependent_startup_configs":)"
       R"([{"Mode_in_machine_instance_refs":)"
       R"([{"Function_group":"MachineState","Mode":"Startup"}]}],)"
       R"("Process_name":"proc2"}]}})"
      },
      {"msm",
       R"({"Application_manifest":{"Application_manifest_id":"msm",)"
       R"("Process":[{"Mode_dependent_startup_configs":)"
       R"([{"Mode_in_machine_instance_refs":[{"Function_group":)"
       R"("MachineState","Mode":"Startup"},{"Function_group":)"
       R"("MachineState","Mode":"Running"}]}],"Process_name":"msm"}]}})"
      }
  };
  for (const auto& appManifest: applicationManifestsInfo)
  {
    std::string path = testConf.corePathToApplicationsFolder
                       + appManifest.first
                       + "/manifest.json";

    writeJson(appManifest.second, path);
  }
}

TEST_F(ManifestReaderTests, ShouldReturnAvailableMachineStates)
{
  ManifestReader reader{testConf};

  auto res = reader.getMachineStates();

  ASSERT_EQ(res,
            (std::vector<MachineState>{"Startup", "Running", "Shutdown"}));
}

TEST_F(ManifestReaderTests, ShouldFaildWhenDirectoryNotExists)
{
  ManifestReader reader{ManifestReaderConf{"./not-exists", "manifest"}};

  EXPECT_THROW(reader.getStatesSupportedByApplication(), std::runtime_error);
}

TEST_F(ManifestReaderTests, ShouldDiscardOtherKeysThanMachineStateWhenParse)
{
  std::string machineManifestWithUnsupportedGroup =
      R"({"Machine_manifest":{"Machine_manifest_id":"Machine",)"
      R"("Mode_declaration_group":[{"Function_group_name":"MachineState",)"
      R"("Mode_declarations":[{"Mode":"Startup"},{"Mode":"Running"},)"
      R"({"Mode":"Shutdown"}]},{"Function_group_name":"TestGroupName",)"
      R"("Mode_declarations":[{"Mode":"tes1"},{"Mode":"tes2"}]}]}})";

  writeJson(machineManifestWithUnsupportedGroup, testConf.machineManifestPath);

  ManifestReader reader{testConf};
  auto res = reader.getMachineStates();

  ASSERT_EQ(res,
            (std::vector<MachineState>{"Startup", "Running", "Shutdown"}));

}

TEST_F(ManifestReaderTests, ShouldEmptyReturnWhenNoMachineStateFunctionGroup)
{
  std::string machineManifestWithoutMachineStates =
      R"({"Machine_manifest":{"Machine_manifest_id":"Machine",)"
      R"("Mode_declaration_group":[{"Function_group_name":"TestGroupName",)"
      R"("Mode_declarations":[{"Mode":"tes1"},{"Mode":"tes2"}]}]}})";

  ManifestReaderConf conf{"./test-data", "./test-data/msm/test-conf.json"};

  writeJson(machineManifestWithoutMachineStates, conf.machineManifestPath);


  ManifestReader reader{conf};
  auto res = reader.getMachineStates();
  ASSERT_EQ(res, emptyMachineStates);

}


TEST_F(ManifestReaderTests, ShouldEmptyReturnWhenEmptyMachineStates)
{
  std::string machineManifestWithoutModeDeclGroups =
      R"({"Machine_manifest":{"Machine_manifest_id":"Machine",)"
      R"("Mode_declaration_group":[]}})";

  ManifestReaderConf conf{"./test-data", "./test-data/msm/test-conf.json"};

  writeJson(machineManifestWithoutModeDeclGroups, conf.machineManifestPath);


  ManifestReader reader{conf};
  auto res = reader.getMachineStates();
  ASSERT_EQ(res, emptyMachineStates);

}


TEST_F(ManifestReaderTests, ShouldReturnMapOfApplicationsForStates)
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

TEST_F(ManifestReaderTests, ShouldDiscardRedundantFunctionGroupsWhenProvided)
{
  std::string input =
      R"({"Application_manifest":{"Application_manifest_id":)"
      R"("app","Process":[{"Mode_dependent_startup_configs":)"
      R"([{"Mode_in_machine_instance_refs":[{"Function_group":"MachineState",)"
      R"("Mode": "Startup"},{"Function_group": "MachineState","Mode":)"
      R"("Running"},{"Function_group": "Test", "Mode": "test"}]}],"Process_name":)"
      R"("app"}]}})";
  system("mkdir ./test-data/app");

  writeJson(input, "./test-data/app/manifest.json");

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

TEST_F(ManifestReaderTests, ShouldEmptyMapWhenNoProvided)
{
  ManifestReaderConf conf{"./empty-manifest/", ""};

  system(("mkdir -p " + conf.corePathToApplicationsFolder + "/app").c_str());

  std::string input =
      R"({"Application_manifest":{"Application_manifest_id":)"
      R"("app","Process":[]}})";

  std::ofstream testOutput{conf.corePathToApplicationsFolder + "/app/manifest.json"};
  testOutput << input;
  testOutput.close();

  ManifestReader reader{conf};
  auto result = reader.getStatesSupportedByApplication();

  ASSERT_EQ(result,
            (std::map<MachineState, std::vector<ProcessName>>{}));

  system(("rm -rf " + conf.corePathToApplicationsFolder).c_str());
}
