#include <json.hpp>
#include <fstream>
#include <dirent.h>
#include <exception>
#include <thread>
#include <signal.h>
#include <iostream>
#include <algorithm>

#include "execution_manager.hpp"


namespace ExecutionManager
{

using std::runtime_error;
using std::string;

const string ExecutionManager::corePath =
  string{"./bin/applications/"};


ExecutionManager::ExecutionManager() : msmProcess({string{"MachineStateManager"}, string{"msm"}})
{
  processManifests();

  startApplication(msmProcess);
}

ExecutionManager::~ExecutionManager()
{
  kill(pidForMsm, SIGTERM);
}

void ExecutionManager::startApplicationsForState()
{
  const auto& allowedApps = allowedApplicationForState.find(currentState);

  if (allowedApps != allowedApplicationForState.cend())
  {
    for (const auto& executableToStart: allowedApps->second)
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
}

void ExecutionManager::killProcessesForState()
{
  auto allowedApps = allowedApplicationForState.find(currentState);

  for (auto app = activeApplications.cbegin(); app != activeApplications.cend();)
  {
    if (allowedApps == allowedApplicationForState.cend() ||
        processToBeKilled(app->first, allowedApps->second))
    {
      kill(app->second, SIGTERM);
      app = activeApplications.erase(app);
    }
    else
    {
      app++;
    }
  }
}

bool ExecutionManager::processToBeKilled(const string& app, const std::vector<ExecutionManager::ProcessName>& allowedApps)
{
  auto it = std::find_if(allowedApps.cbegin(),
                     allowedApps.cend(),
                     [&app](auto& listItem)
    { return app == listItem.processName; });

  return (it  == allowedApps.cend());
};

std::vector<string> ExecutionManager::loadListOfApplications()
{
  DIR* dp = nullptr;
  std::vector<string> fileNames;

  if ((dp = opendir(corePath.c_str())) == nullptr)
  {
    throw runtime_error(string{"ExecutionManager: Error opening directory: "}
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
      throw runtime_error(string{"ExecutionManager: Error occured creating process: "}
                          + process.processName
                          + " "
                          + strerror(errno));
    }
  } else {
    // parent process
    if (process.processName == "msm")
    {
      pidForMsm = processId;
    }
    else
    {
      activeApplications.insert({process.processName, processId});
    }
  }

}

::kj::Promise<void>
ExecutionManager::reportApplicationState(ReportApplicationStateContext context)
{
  ApplicationState state = context.getParams().getState();

  std::cout << "ExecutionManager: State #" << static_cast<uint16_t>(state)
            << " received"
            << std::endl;

  return kj::READY_NOW;
}

::kj::Promise<void>
ExecutionManager::register_(RegisterContext context)
{
  string newMachineClient = context.getParams().getAppName();

  if (machineStateClientAppName.empty() ||
      machineStateClientAppName == newMachineClient)
  {
    machineStateClientAppName = newMachineClient;
    context.getResults().setResult(StateError::K_SUCCESS);

    std::cout << "ExecutionManager: State Machine Client \""
              << machineStateClientAppName
              << "\" registered"
              << std::endl;
    
    //removing msm from allowedApplicationForState
    for (auto& pair:allowedApplicationForState)
    {
     
        
      auto toBeRemoved = std::remove_if(pair.second.begin(),
                                    pair.second.end(),
                                    [&](ProcessName & processName){
                                      return (processName.applicationName == machineStateClientAppName);
                                    }
                                  ); 
      
      
      pair.second.erase(toBeRemoved, pair.second.end());
    }
  }
  else
  {
    context.getResults().setResult(StateError::K_INVALID_REQUEST);

    std::cout << "ExecutionManager: State Machine Client \""
              << machineStateClientAppName
              << "\" registration failed" 
              << std::endl;
  }

  return kj::READY_NOW;
}

::kj::Promise<void>
ExecutionManager::getMachineState(GetMachineStateContext context)
{
  std::cout << "ExecutionManager: getMachineState request received" << std::endl;

  context.getResults().setState(currentState);

  context.getResults().setResult(StateError::K_SUCCESS);

  return kj::READY_NOW;
}

::kj::Promise<void>
ExecutionManager::setMachineState(SetMachineStateContext context)
{
  string state = context.getParams().getState().cStr();

  if (!state.empty() && state != currentState)
  {
    currentState = state;

    killProcessesForState();

    startApplicationsForState();
   
    context.getResults().setResult(StateError::K_SUCCESS);

    std::cout << "ExecutionManager: Machine state changed successfully to "
              << "\"" 
              << currentState << "\"" 
              << std::endl;
  }
  else
  {
    context.getResults().setResult(StateError::K_INVALID_STATE);

    std::cout << "ExecutionManager: Invalid machine state received - "
              << "\"" << currentState << "\"" 
              << std::endl;
  }

  return kj::READY_NOW;
}

} // namespace ExecutionManager
