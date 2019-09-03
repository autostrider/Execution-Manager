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

const std::vector<MachineState> ExecutionManager::transition =
  {"init", "running", "shutdown"};


ExecutionManager::ExecutionManager()
{
  processManifests();
}

std::int32_t ExecutionManager::start()
{
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

bool ExecutionManager::processToBeKilled(const string& app, const std::vector<ProcessInfo>& allowedApps)
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
    throw runtime_error(string{"Error opening directory: "}
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

std::vector<char *>
ExecutionManager::getArgumentsList(const ExecutionManager::ProcessInfo& process) const
{
  std::vector<std::string> argv;
  std::transform(process.startOptions.cbegin(),
                 process.startOptions.cend(),
                 std::back_inserter(argv),
                 [](const StartupOption& option)
  { return option.makeCommandLineOption(); });

  // insert app name
  argv.insert(argv.begin(), process.processName);

  std::vector<char*> nptArgv;
  // include terminating sign, that not included in argv
  nptArgv.reserve(argv.size() + 1);

  for(auto& str: argv)
  {
    nptArgv.push_back(&str[0]);
  }

  // terminating sign
  nptArgv.push_back(nullptr);

  return nptArgv;
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
        for (const auto& opt: conf.startupOptions)
        {
          std::cout << opt.optionArg << std::endl;
        }
        for (const auto& mode: conf.modes)
        {
          if (mode.functionGroup != "MachineState")
            continue;

          allowedApplicationForState[mode.mode]
              .push_back({manifest.manifest.manifestId,
                          process.name,
                          conf.startupOptions});
        }
      }
    }
  }
}

void ExecutionManager::startApplication(const ProcessInfo& process)
{
  pid_t processId = fork();

  if (!processId)
  {
    // child process
    auto processPath = corePath + process.applicationName + "/processes/" + process.processName;

    const auto& argv = getArgumentsList(process);

    int res = execv(processPath.c_str(), argv.data());

    if (res)
    {
      throw runtime_error(string{"Error occured creating process: "}
                          + process.processName
                          + " "
                          + strerror(errno));
    }
  }
  else
  {
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
