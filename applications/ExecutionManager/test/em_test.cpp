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
