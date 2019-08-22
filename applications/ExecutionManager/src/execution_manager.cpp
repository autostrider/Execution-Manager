#include "execution_manager.hpp"

#include <json.hpp>
#include <fstream>
#include <dirent.h>
#include <exception>
#include <thread>
#include <signal.h>


#include <iostream>


namespace ExecutionManager
{

using std::runtime_error;

const std::string ExecutionManager::corePath = "../applications/AdaptiveApplications/";

const std::vector<MachineState> ExecutionManager::transition =
{"init", "running", "shutdown"};

int ExecutionManager::start()
{
  processManifests();

  for (const auto& state: transition)
  {
    std::cout << "————————————————————————————————————————————————————————\n";
    currentState = state;

    killProcessesForState();
    std::cout << state << std::endl;

    startApplicationsForState();

    std::this_thread::sleep_for(std::chrono::seconds{2});
  }

  return EXIT_SUCCESS;
}

void ExecutionManager::startApplicationsForState()
{
  const auto& allowedApps = allowedApplicationForState[currentState];

  for (const auto& executableToStart: allowedApps)
  {
    if (activeApplications.find(executableToStart.processName) != activeApplications.cend())
    {
      continue;
    }
    try
    {
      startApplication(executableToStart);
    }
    catch (const runtime_error& err)
    {
      std::cout << err.what() << std::endl;
    }
  }
}

void ExecutionManager::killProcessesForState()
{
  const auto& allowedApps = allowedApplicationForState[currentState];

  for (auto app = activeApplications.cbegin(); app != activeApplications.cend();)
  {
    if (std::find_if(allowedApps.cbegin(),
                     allowedApps.cend(),
                     [&app](auto& allowedApp)
    { return app->first == allowedApp.processName; }) == allowedApps.cend())
    {
      kill(app->second, SIGTERM);
      app = activeApplications.erase(app);
    } else {
      app++;
    }
  }
}

std::vector<std::string> ExecutionManager::loadListOfApplications()
{
  DIR* dp = nullptr;
  std::vector<std::string> fileNames;

  if ((dp = opendir(corePath.c_str())) == nullptr)
  {
    throw runtime_error(std::string{"Error opening directory: "}
                        + corePath
                        + " "
                        + strerror(errno));
  }

  for (struct dirent *drnt = readdir(dp); drnt != nullptr; drnt = readdir(dp))
  {
    // check for "." and ".." files in directory, we don't need them
    if (!strcmp(drnt->d_name, ".") || !strcmp(drnt->d_name, "..")) continue;

    fileNames.emplace_back(drnt->d_name);

    std::cout << drnt->d_name << std::endl;
  }

  closedir(dp);
  return fileNames;
}

void ExecutionManager::processManifests()
{
  const auto& applicationNames = loadListOfApplications();

  json content;
  for (auto file: applicationNames)
  {
    file = corePath + file + "/manifest.json";
    ifstream data{file};

    data >> content;
    ApplicationManifest manifest = content;

    for (const auto& process: manifest.manifest.processes)
    {
      for (const auto& conf: process.modeDependentStartupConf)
      {
        for (const auto& mode: conf.modes)
        {
          if (mode.functionGroup != "MachineState")
            continue;

          allowedApplicationForState[mode.mode].push_back({manifest.manifest.manifestId, process.name});
        }
      }
    }

  }
}

void ExecutionManager::startApplication(const ProcessName& process)
{
  pid_t processId = fork();

  if (!processId)
  {
    // child process
    auto processPath = corePath + process.applicationName + "/processes/" + process.processName;

    int res = execl(processPath.c_str(), process.processName.c_str(), nullptr);

    if (res)
    {
      throw runtime_error(std::string{"Error occured creating process: "}
                          + process.processName
                          + " "
                          + strerror(errno));
    }
  } else {
    // parent process
    activeApplications.insert({process.processName, processId});
  }

}

} // namespace ExecutionManager
