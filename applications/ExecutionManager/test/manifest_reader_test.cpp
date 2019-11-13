#include "manifests.hpp"
#include "manifest_reader.hpp"

#include <iostream>
#include <string>
#include <fstream>

#include "gtest/gtest.h"

namespace ExecutionManagerTests
{

using namespace ExecutionManager;
using nlohmann::json;

/// Config data structures started
struct ManifestReaderTestConf
{
  ManifestReaderConf conf;
  std::vector<std::string> nestedDirs;
};

struct MachineManifestReaderTestConf : public ManifestReaderTestConf
{
  MachineManifestReaderTestConf(const MachineManifest& machineManifest,
                                const ManifestReaderConf& manifestConf =
      ManifestReaderConf{"./test-data/", "./test-data/msm/machine_manifest.json"},
      const std::vector<std::string>& testNestedDirs = {"msm"})
    : ManifestReaderTestConf{manifestConf, testNestedDirs}
    , machineManifest{machineManifest}
  {}

  MachineManifest machineManifest;
};

struct ApplicationManifestReaderTestConf: public ManifestReaderTestConf
{
  ApplicationManifestReaderTestConf(const std::map<std::string, ApplicationManifest> applicationManifests,
                                    const std::vector<std::string>& testNestedDirs,
                                    const ManifestReaderConf& testConf =
      ManifestReaderConf{"./test-data/", "./test-data/msm/machine_manifest.json"})
    : ManifestReaderTestConf{testConf, testNestedDirs}
    ,  applicationManifests{applicationManifests}
  {}

  const std::map<std::string, ApplicationManifest> applicationManifests;
};
/// config data structures finished

namespace ConfigMethods
{

void writeJson(const json& content, const std::string& path)
{
  std::ofstream output{path};
  output << content;
}

void createDirs(const ManifestReaderTestConf& readerTestConf)
{
    std::string createDir = "mkdir ";

    std::string createCoreDir = createDir + readerTestConf.conf.pathToApplicationsFolder;
    system(createCoreDir.c_str());

    for (const auto& dir: readerTestConf.nestedDirs)
    {
      std::string createNestedDir = createDir
                                    + readerTestConf.conf.pathToApplicationsFolder
                                    + dir;

      system(createNestedDir.c_str());
    }
}

void removeDirs(const ManifestReaderTestConf& readerTestConf)
{
  std::string rmTestDir = "rm -rf " + readerTestConf.conf.pathToApplicationsFolder;
  system(rmTestDir.c_str());
}

} // namespace ConfigMethods

/// Manifest Tests declaration Started
class MachineManifestReadingTests :
    public ::testing::TestWithParam<MachineManifestReaderTestConf>
{
protected:
  void SetUp() override;
  void TearDown() override;

  const std::vector<MachineState> emptyMachineStates = {};
  const std::vector<MachineState> availableMachineStates =
    {"Startup", "Running", "Shutdown"};
private:
  void createMachineManifest();
};

class EmptyMachineManifestReadingTests :
    public MachineManifestReadingTests
{};

class ApplicationManifestReadingTests :
    public ::testing::TestWithParam<ApplicationManifestReaderTestConf>
{
protected:
  void SetUp() override;
  void TearDown() override;

  const std::map<MachineState, ProcessesList>
    availableAppsForStates =
      {
        {"Startup", { "msm_msm", "test-aa2_proc2"} },
        {"Running", { "msm_msm", "test-aa1_proc1"} },
      };
  const std::map<MachineState, ProcessesList> emptyAppsForStates =
    {};
private:
  void createApplicationManifests();
};

class EmptyApplicationManifestReadingTests :
    public ApplicationManifestReadingTests
{};

class ApplicationCommandLineOptionsReadingTests:
    public ApplicationManifestReadingTests
{
protected:
  const std::vector<std::string> arguments =
    {"-name argument",
     "--name=argument",
     "name",
     "-name",
     "--name"};
};
/// manifest tests declarations finished

void ApplicationManifestReadingTests::SetUp()
{
  ConfigMethods::createDirs(GetParam());
  createApplicationManifests();
}

void ApplicationManifestReadingTests::TearDown()
{
  ConfigMethods::removeDirs(GetParam());
}

void MachineManifestReadingTests::SetUp()
{
  ConfigMethods::createDirs(GetParam());
  createMachineManifest();
}

void MachineManifestReadingTests::TearDown()
{
 ConfigMethods::removeDirs(GetParam());
}

void MachineManifestReadingTests::createMachineManifest()
{
  ConfigMethods::writeJson(GetParam().machineManifest,
                           GetParam().conf.machineManifestFilePath);
}

void ApplicationManifestReadingTests::createApplicationManifests()
{
  for (const auto& appManifest: GetParam().applicationManifests)
  {
    std::string path = GetParam().conf.pathToApplicationsFolder
                       + appManifest.first
                       + "/manifest.json";

    ConfigMethods::writeJson(appManifest.second, path);
  }
}

const MachineManifestReaderTestConf regularMachineManifestTestConf =
  {
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
};

const MachineManifestReaderTestConf
  machineManifestWithAdditionalFunctionGroup =
  {
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
};

const MachineManifestReaderTestConf emptyMachineManifest =
  {
    MachineManifest{
      MachineManifestInternal{
        "Machine",
        { }
      }
    }
  };

const ApplicationManifestReaderTestConf regularApplicationManifestsConf =
 {
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
    }, // "test-aa1" application manifest & folder
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
    }, // "test-aa2" application manifest & folder
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
  }, // "msm" application manifest & folder
  {"test-aa1", "test-aa2", "msm"}
};

const ApplicationManifestReaderTestConf emptyApplicationManifest =
 {
    {
      {
        "app",
        ApplicationManifest{
          ApplicationManifestInternal{
            "app",
            {}
          }
        }
      } // "app" application
    }, // ApplicationManifest
    {"app"}
};

const ApplicationManifestReaderTestConf
  additionalFunctionGroupsApplicationManifest =
  {
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
  }, // "test-aa1" application manifest & folder
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
  }, // "test-aa2" application manifest & folder
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
}, // "msm" application manifest & folder
{"test-aa1", "test-aa2", "msm"}
};

const ApplicationManifestReaderTestConf commandLineOptionShortForm =
{
   {
     {
       "app",
       ApplicationManifest{
         ApplicationManifestInternal{
           "app",
           {Process{
              "app",
              {
                ModeDepStartupConfig{
                  {
                    MachineInstanceMode{"MachineState", "Startup"},
                  },
                  {
                    StartupOption{
                      StartupOptionKindEnum::commandLineShortForm,
                      "name",
                      "argument"
                    }
                  }
                }
              }
            }
          }
         }
       }
     } // "app" application
   }, // ApplicationManifest
   {"app"}
};

const ApplicationManifestReaderTestConf commandLineOptionShortFormWithoutArg =
{
   {
     {
       "app",
       ApplicationManifest{
         ApplicationManifestInternal{
           "app",
           {Process{
              "app",
              {
                ModeDepStartupConfig{
                  {
                    MachineInstanceMode{"MachineState", "Startup"},
                  },
                  {
                    StartupOption{
                      StartupOptionKindEnum::commandLineShortForm,
                      "name",
                      ""
                    }
                  }
                }
              }
            }
          }
         }
       }
     } // "app" application
   }, // ApplicationManifest
   {"app"}
};

const ApplicationManifestReaderTestConf commandLineOptionSimpleForm =
{
   {
     {
       "app",
       ApplicationManifest{
         ApplicationManifestInternal{
           "app",
           {Process{
              "app",
              {
                ModeDepStartupConfig{
                  {
                    MachineInstanceMode{"MachineState", "Startup"},
                  },
                  {
                    StartupOption{
                      StartupOptionKindEnum::commandLineSimpleForm,
                      "name",
                      "argument"
                    }
                  }
                }
              }
            }
          }
         }
       }
     } // "app" application
   }, // ApplicationManifest
   {"app"}
};

const ApplicationManifestReaderTestConf commandLineOptionLongForm =
{
   {
     {
       "app",
       ApplicationManifest{
         ApplicationManifestInternal{
           "app",
           {Process{
              "app",
              {
                ModeDepStartupConfig{
                  {
                    MachineInstanceMode{"MachineState", "Startup"},
                  },
                  {
                    StartupOption{
                      StartupOptionKindEnum::commandLineLongForm,
                      "name",
                      "argument"
                    }
                  }
                }
              }
            }
          }
         }
       }
     } // "app" application
   }, // ApplicationManifest
   {"app"}
};

const ApplicationManifestReaderTestConf commandLineOptionLongFormWithoutArg =
{
   {
     {
       "app",
       ApplicationManifest{
         ApplicationManifestInternal{
           "app",
           {Process{
              "app",
              {
                ModeDepStartupConfig{
                  {
                    MachineInstanceMode{"MachineState", "Startup"},
                  },
                  {
                    StartupOption{
                      StartupOptionKindEnum::commandLineLongForm,
                      "name",
                      ""
                    }
                  }
                }
              }
            }
          }
         }
       }
     } // "app" application
   }, // ApplicationManifest
   {"app"}
};

const std::vector<MachineManifestReaderTestConf> machineManifestTestsConfigs =
{
  regularMachineManifestTestConf,
  machineManifestWithAdditionalFunctionGroup,
};

const std::vector<ApplicationManifestReaderTestConf> applicationManifestReadingTestConf =
{
  regularApplicationManifestsConf,
  additionalFunctionGroupsApplicationManifest,
};

const std::vector<ApplicationManifestReaderTestConf> commandLineOptions =
{
  commandLineOptionShortForm,
  commandLineOptionLongForm,
  commandLineOptionSimpleForm,
  commandLineOptionShortFormWithoutArg,
  commandLineOptionLongFormWithoutArg
};

INSTANTIATE_TEST_SUITE_P(ApplicationManifestTests,
                         ApplicationManifestReadingTests,
                         ::testing::ValuesIn(applicationManifestReadingTestConf));

INSTANTIATE_TEST_SUITE_P(MachineManifestTests,
                         MachineManifestReadingTests,
                         ::testing::ValuesIn(machineManifestTestsConfigs));

INSTANTIATE_TEST_SUITE_P(EmptyMachineManifestTests,
                         EmptyMachineManifestReadingTests,
                         ::testing::Values(emptyMachineManifest));

INSTANTIATE_TEST_SUITE_P(
    EmptyApplicationManifestParsing,
    EmptyApplicationManifestReadingTests,
    ::testing::Values(emptyApplicationManifest));

INSTANTIATE_TEST_SUITE_P(
    ApplicationCommandLineArgumentsParsing,
    ApplicationCommandLineOptionsReadingTests,
    ::testing::ValuesIn(commandLineOptions));

TEST_P(MachineManifestReadingTests,
       ShouldReturnAvailableMachineStatesWhenProvided)
{
  ManifestReader reader{GetParam().conf};

  auto res = reader.getMachineStates();

  ASSERT_EQ(res, availableMachineStates);
}

TEST_P(EmptyMachineManifestReadingTests,
       ShouldReturnEmptyMachineStatesWhenNoProvided)
{
  ManifestReader reader{GetParam().conf};

  auto res = reader.getMachineStates();

  ASSERT_EQ(res, emptyMachineStates);
}

TEST_P(ApplicationManifestReadingTests,
       ShouldReturnMapOfApplicationsForStatesWhenProvided)
{
  const auto& testParam = GetParam();
  ManifestReader reader{testParam.conf};

  auto result = reader.getStatesSupportedByApplication();

    EXPECT_EQ(result, availableAppsForStates);
}

TEST_P(EmptyApplicationManifestReadingTests,
       ShouldReturnEmptyAppsForStateWhenNoProvided)
{
  const auto& testParam = GetParam();
  ManifestReader reader{testParam.conf};

  auto result = reader.getStatesSupportedByApplication();

  EXPECT_EQ(result, emptyAppsForStates);
}

TEST(ApplicationManifestReadingTestsFail, ShouldFaildWhenDirectoryNotExists)
{
  ManifestReader reader{ManifestReaderConf{"./not-exists", "manifest"}};

  EXPECT_THROW(reader.getStatesSupportedByApplication(), std::runtime_error);
}

} // namespace ExecutionManagerTests
