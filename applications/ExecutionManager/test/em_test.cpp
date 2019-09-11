#include "manifests.hpp"
#include "manifest_reader.hpp"
#include "gtest/gtest.h"
#include <iostream>
#include <string>
#include <sys/stat.h>
#include <unistd.h>

using namespace ExecutionManager;
using nlohmann::json;

struct ManifestReaderTestConf
{
  std::vector<std::string> nestedDirs;
  std::map<std::string, ApplicationManifest> applicationManifests;
  MachineManifest machineManifest;
  ManifestReaderConf conf{"./test-data/",
                          "./test-data/msm/machine_manifest.json"};
};

class ManifestReaderTests : public ::testing::TestWithParam<ManifestReaderTestConf>
{
protected:
  void SetUp() override;
  void TearDown() override;
protected:
  void writeJson(const json& content, const std::string& path);
private:
  void createDirs();

};

class MachineManifestReadingTests : public ManifestReaderTests
{
protected:
  void SetUp() override;

  const std::vector<MachineState> emptyMachineStates = {};
  const std::vector<MachineState> availableMachineStates =
    {"Startup", "Running", "Shutdown"};
private:
  void createMachineManifest();
};

class ApplicationManifestReadingTests : public ManifestReaderTests
{
protected:
  void SetUp() override;

  const std::map<MachineState, std::vector<ProcessInfo>> availableAppsForStates =
  {
      {"Startup", {
         ProcessInfo{"msm", "msm", {}},
         ProcessInfo{"test-aa2", "proc2", {}},
                   }},
      {"Running", {
         ProcessInfo{"test-aa1", "proc1", {}},
         ProcessInfo{"msm", "msm", {}},
                  }},
  };
  const std::map<MachineState, std::vector<ProcessInfo>> emptyAppsForStates = {};
private:
  void createApplicationManifests();
};

void ApplicationManifestReadingTests::SetUp()
{
  ManifestReaderTests::SetUp();
  createApplicationManifests();
}

void MachineManifestReadingTests::SetUp()
{
  ManifestReaderTests::SetUp();
  createMachineManifest();
}

void ManifestReaderTests::SetUp()
{
  createDirs();
}

void ManifestReaderTests::TearDown()
{
    std::string rmTestDir = "rm -rf " + GetParam().conf.pathToApplicationsFolder;
    system(rmTestDir.c_str());
}

void ManifestReaderTests::writeJson(const json& content, const std::string& path)
{
  std::ofstream output{path};
  output << content;
}

void ManifestReaderTests::createDirs()
{
    std::string createDir = "mkdir ";

    std::string createCoreDir = createDir + GetParam().conf.pathToApplicationsFolder;
    system(createCoreDir.c_str());

    for (const auto& dir: GetParam().nestedDirs)
    {
      std::string createNestedDir = createDir
                                    + GetParam().conf.pathToApplicationsFolder
                                    + dir;

      system(createNestedDir.c_str());
    }
}

void MachineManifestReadingTests::createMachineManifest()
{
  writeJson(GetParam().machineManifest, GetParam().conf.machineManifestFilePath);
}

void ApplicationManifestReadingTests::createApplicationManifests()
{
  for (const auto& appManifest: GetParam().applicationManifests)
  {
    std::string path = GetParam().conf.pathToApplicationsFolder
                       + appManifest.first
                       + "/manifest.json";

    writeJson(appManifest.second, path);
  }
}

const std::vector<ManifestReaderTestConf> machineManifestTestsConfigs =
{
  ManifestReaderTestConf{
    {"msm"},
    {},
    MachineManifest{
      MachineManifestInternal{
        "Machine",
        {
          ModeDeclarationGroup{
            "MachineState",
            {Mode{"Startup"}, Mode{"Running"}, Mode{"Shutdown"}}
          }
        }
      }
    }
  },
  ManifestReaderTestConf{
    {"msm"},
    {},
    MachineManifest{
      MachineManifestInternal{
        "Machine",
        {
          ModeDeclarationGroup{
            "MachineState",
            {Mode{"Startup"}, Mode{"Running"}, Mode{"Shutdown"}}
          },
          ModeDeclarationGroup{
              "TestGroup",
            {Mode{"test1"}, Mode{"test2"}}
          }
        }
      }
    }
  },
  ManifestReaderTestConf{
    {"msm"},
    {},
    MachineManifest{
      MachineManifestInternal{
        "Machine",
        { }
      }
    }
  }
};

INSTANTIATE_TEST_SUITE_P(MachineManifestTests,
                         MachineManifestReadingTests,
                         ::testing::ValuesIn(machineManifestTestsConfigs));

TEST_P(MachineManifestReadingTests, ShouldReturnAvailableMachineStatesWhenProvided)
{
  ManifestReader reader{GetParam().conf};

  auto res = reader.getMachineStates();

  if (GetParam().machineManifest.manifest.modeDeclarationGroups.size())
  {
    ASSERT_EQ(res, availableMachineStates);
  }
  else
  {
    ASSERT_EQ(res, emptyMachineStates);
  }
}

TEST(ApplicationManifestReadingTestsFail, ShouldFaildWhenDirectoryNotExists)
{
  ManifestReader reader{ManifestReaderConf{"./not-exists", "manifest"}};

  EXPECT_THROW(reader.getStatesSupportedByApplication(), std::runtime_error);
}

const std::vector<ManifestReaderTestConf> applicationManifestReadintTestConf =
{
  ManifestReaderTestConf{
    {"test-aa1", "test-aa2", "msm"},
    {
      {
        "test-aa1",
        ApplicationManifest{
          ApplicationManifestInternal{
            "test-aa1",
            {
              Process{
                "proc1",
                {
                  ModeDepStartupConfig{
                    {
                      MachineInstanceMode{"MachineState", "Running"}
                    }, {}
                  }
                }
              }
            }
          }
        }
      },
      {
        "test-aa2",
        ApplicationManifest{
          ApplicationManifestInternal{
            "test-aa2",
            {
              Process{
                "proc2",
                {
                  ModeDepStartupConfig{
                    {
                      MachineInstanceMode{"MachineState", "Startup"}
                    }, {}
                  }
                }
              }
            }
          }
        }
      },
      {
        "msm",
        ApplicationManifest{
          ApplicationManifestInternal{
            "msm",
            {
              Process{
                "msm",
                {
                  ModeDepStartupConfig{
                    {
                      MachineInstanceMode{"MachineState", "Startup"},
                      MachineInstanceMode{"MachineState", "Running"}
                    }, {}
                  }
                }
              }
            }
          }
        }
      }
    },
    {} // MachineManifest
  },
  ManifestReaderTestConf{
    {"app"},
    {
      {
        "app",
        ApplicationManifest{
          ApplicationManifestInternal{
            "app",
            {}
          }
        }
      }
    }, //ApplicationManifest
    {} // MachineManifest
  },
  ManifestReaderTestConf{
    {"test-aa1", "test-aa2", "msm"},
    {
      {
        "test-aa1",
        ApplicationManifest{
          ApplicationManifestInternal{
            "test-aa1",
            {
              Process{
                "proc1",
                {
                  ModeDepStartupConfig{
                    {
                      MachineInstanceMode{"MachineState", "Running"},
                      MachineInstanceMode{"TestMode", "testData"}
                    }, {}
                  }
                }
              }
            }
          }
        }
      },
      {
        "test-aa2",
        ApplicationManifest{
          ApplicationManifestInternal{
            "test-aa2",
            {
              Process{
                "proc2",
                {
                  ModeDepStartupConfig{
                    {
                      MachineInstanceMode{"MachineState", "Startup"},
                      MachineInstanceMode{"TestMode", "testData"}
                    }, {}
                  }
                }
              }
            }
          }
        }
      },
      {
        "msm",
        ApplicationManifest{
          ApplicationManifestInternal{
            "msm",
            {
              Process{
                "msm",
                {
                  ModeDepStartupConfig{
                    {
                      MachineInstanceMode{"MachineState", "Startup"},
                      MachineInstanceMode{"MachineState", "Running"}
                    }, {}
                  }
                }
              }
            }
          }
        }
      }
    },
    {} // MachineManifest
  },
};

TEST_P(ApplicationManifestReadingTests,
       ShouldReturnMapOfApplicationsForStatesWhenProvided)
{
  const auto& testParam = GetParam();
  ManifestReader reader{testParam.conf};

  auto result = reader.getStatesSupportedByApplication();

  if (testParam.applicationManifests.find("app") ==
      testParam.applicationManifests.cend())
  {
    EXPECT_EQ(result, availableAppsForStates);
  }
  else
  {
    EXPECT_EQ(result, emptyAppsForStates);
  }
}
