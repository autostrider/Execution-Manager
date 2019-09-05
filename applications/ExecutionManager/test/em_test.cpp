#include "manifests.hpp"
#include "manifest_reader.hpp"
#include "gtest/gtest.h"
#include <iostream>
#include <string>
#include <sys/stat.h>
#include <unistd.h>

using namespace ExecutionManager;
using nlohmann::json;

namespace ManifestReaderTestData
{

const ManifestReader reader;
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

std::vector<std::string> nestedDirs =
  {"msm", "test-aa1", "test-aa2"};

const std::string machineManifestPath =
  reader.corePath + "/" + nestedDirs.at(0) + "/machine_manifest.json";

const std::map<std::string, ApplicationManifest> applicationManifestsInfo =
  {
  {"test-aa1",
     ApplicationManifest{
       ApplicationManifestInternal{
         "test-aa1",
         {Process{
           "proc1",
           {ModeDepStartupConfig{
             {MachineInstanceMode{"MachineManifest", "Running"}
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
             {MachineInstanceMode{"MachineManifest", "Startup"}
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
             {MachineInstanceMode{"MachineManifest", "Startup"},
              MachineInstanceMode{"MachineManifest", "Running"}
             }}}
         }}
       }
     }
    }
};
} // namespace ManifestReaderTestData

class ManifestReaderEnvironment : public ::testing::Environment
{
public:
  ~ManifestReaderEnvironment() override = default;

  void SetUp() override;

  void TearDown() override;

private:
  void createMachineManifest();

  void createDirs();

  void createApplicationManifests();

};

void ManifestReaderEnvironment::SetUp()
{
    createDirs();
    createMachineManifest();
    createApplicationManifests();
}

void ManifestReaderEnvironment::TearDown()
{
  std::string rmTestDir = "rm -rf " + ManifestReaderTestData::reader.corePath;
  system(rmTestDir.c_str());
}

void ManifestReaderEnvironment::createMachineManifest()
{
  json manifSerialized = ManifestReaderTestData::machineManifest;

    std::ofstream output;
    output.open(ManifestReaderTestData::machineManifestPath);
    output << manifSerialized;
}

void ManifestReaderEnvironment::createDirs()
{
    std::string createDir = "mkdir ";

    std::string createCoreDir = createDir + ManifestReaderTestData::reader.corePath;
    system(createCoreDir.c_str());

    for (const auto& dir: ManifestReaderTestData::nestedDirs)
    {
      std::string createNestedDir = createDir
                                    + ManifestReaderTestData::reader.corePath
                                    + dir;

      system(createNestedDir.c_str());
    }
    system("ls -la ./test-data");
}

void ManifestReaderEnvironment::createApplicationManifests()
{
  for (const auto& appManifest: ManifestReaderTestData::applicationManifestsInfo)
  {
    std::string path = ManifestReaderTestData::reader.corePath
                       + appManifest.first
                       + "/manifest.json";
    std::ofstream output{path};

    json applicationManifestForPath = appManifest.second;
    output << applicationManifestForPath;
  }

  system(("tree " + ManifestReaderTestData::reader.corePath).c_str());
}

/// INIT ENVIRONMENT
::testing::Environment* const manifestReaderEnvironment =
    ::testing::AddGlobalTestEnvironment(new ManifestReaderEnvironment);
/// -------------------

TEST(ManifestReaderTest, getJsonDataTest)
{
  ManifestReader reader;

  json manifSerialized = ManifestReaderTestData::machineManifest;

  json returnObj = reader.getJsonData(ManifestReaderTestData::machineManifestPath);

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
  std::sort(ManifestReaderTestData::nestedDirs.begin(),
            ManifestReaderTestData::nestedDirs.end());

  std::sort(resultAppsList.begin(),
            resultAppsList.end());

  ASSERT_EQ(ManifestReaderTestData::nestedDirs, resultAppsList);
}

TEST(ManifestReaderTest, getStatesSupportedByApplicationTest)
{
  ManifestReader reader;

  const auto comp = [](const ProcessName& proc1, const ProcessName& proc2)
  { return proc1.processName < proc2.processName; };

  std::map<MachineState, std::vector<ProcessName>> expectedResult =
  {
      {"Startup", {ProcessName{"test-aa1", "proc1"},
                   ProcessName{"msm", "msm"}}},
      {"Running", {ProcessName{"test-aa2", "proc2"},
                   ProcessName{"msm", "msm"}}},
  };

  for (auto& expAppsForState: expectedResult)
  {
    std::sort(expAppsForState.second.begin(),
              expAppsForState.second.end(),
              comp);
  }

  auto result = reader.getStatesSupportedByApplication();
  for (auto& appsForState: result)
  {
    std::sort(appsForState.second.begin(),
              appsForState.second.end(),
              comp);

    ASSERT_EQ(appsForState.second.size(),
              expectedResult[appsForState.first].size());

    const auto& expResultForState = expectedResult[appsForState.first];

    for (size_t i = 0; i < expResultForState.size(); i++)
    {
      ASSERT_EQ(appsForState.second.at(i),
                expResultForState.at(i));
    }
  }
}
