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
using std::string;

const string ExecutionManager::corePath =
  string{"./bin/applications/"};

int32_t ExecutionManager::start()
{
  processManifests();

  try
  {
    capnp::EzRpcServer server(kj::heap(std::move(*this)),
                              "unix:/tmp/execution_management");
    auto &waitScope = server.getWaitScope();

    std::cout << "Execution Manager started.." << std::endl;

    kj::NEVER_DONE.wait(waitScope);
  }
  catch (const kj::Exception &e)
  {
    std::cerr << e.getDescription().cStr() << std::endl;
  }

  return EXIT_SUCCESS;
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
  const auto& allowedApps = allowedApplicationForState.find(currentState);

  for (auto app = activeApplications.cbegin(); app != activeApplications.cend();)
  {
    if (allowedApps == allowedApplicationForState.cend() ||
        std::find_if(allowedApps->second.cbegin(),
                     allowedApps->second.cend(),
                     [&app](auto& allowedApp)
    { return app->first == allowedApp.processName; }) == allowedApps->second.cend())
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
          std::cout<<"pushing to allowedApplicationForState  process.name =" <<  process.name << std::endl;
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
    std::string processPath = corePath + process.applicationName + std::string("/processes/") + process.processName;
    
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

::kj::Promise<void>
ExecutionManager::reportApplicationState(ReportApplicationStateContext context)
{
  ApplicationState state = context.getParams().getState();

  std::cout << "State #" << static_cast<uint16_t>(state)
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

    std::cout << "State Machine Client \""
              << machineStateClientAppName
              << "\" registered"
              << std::endl;
  }
  else
  {
    context.getResults().setResult(StateError::K_INVALID_REQUEST);

    std::cout << "State Machine Client \""
              << machineStateClientAppName
              << "\" registration failed" 
              << std::endl;
  }

  return kj::READY_NOW;
}

::kj::Promise<void>
ExecutionManager::getMachineState(GetMachineStateContext context)
{
  std::cout << "getMachineState request received" << std::endl;

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

    std::cout << "Machine state changed successfully to "
              << "\"" 
              << currentState << "\"" 
              << std::endl;
  }
  else
  {
    context.getResults().setResult(StateError::K_INVALID_STATE);

    std::cout << "Invalid machine state received - "
              << "\"" << currentState << "\"" 
              << std::endl;
  }

  return kj::READY_NOW;
}

} // namespace ExecutionManager
