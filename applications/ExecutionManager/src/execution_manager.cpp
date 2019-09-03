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

ExecutionManager::ExecutionManager(std::unique_ptr<IManifestReader> reader)
  : m_activeApplications{}
  , m_allowedApplicationForState{reader->getApplicationStatesMap()}
  , m_currentState{}
  , m_machineManifestStates{reader->getMachineStates()}
  , machineStateClientAppName{}
{
  filterStates();
}

int32_t ExecutionManager::start()
{
  for (const auto& allowedItem: m_allowedApplicationForState)
  {
    for (const auto& item: allowedItem.second)
    {
      std::cout << allowedItem.first << "\t" << item.processName << "\n";
    }
  }

  for (const auto& state: m_machineManifestStates)
  {
    std::cout << "————————————————————————————————————————————————————————\n";
    m_currentState = state;

    killProcessesForState();
    std::cout << state << std::endl;

    startApplicationsForState();

    std::this_thread::sleep_for(std::chrono::seconds{2});
  }

    std::cout << "Execution Manager started.." << std::endl;

  return EXIT_SUCCESS;
}

void ExecutionManager::filterStates()
{
  for (auto app = m_activeApplications.begin(); app != m_activeApplications.end();)
  {
    if (std::find(m_machineManifestStates.cbegin(),
                  m_machineManifestStates.cend(),
                  app->first) == m_machineManifestStates.cend())
    {
      app = m_activeApplications.erase(app);
    }
    else
    {
      app++;
    }
  }
}

void ExecutionManager::startApplicationsForState()
{
  const auto& allowedApps = m_allowedApplicationForState.find(m_currentState);

  if (allowedApps != m_allowedApplicationForState.cend())
  {
    for (const auto& executableToStart: allowedApps->second)
    {
      if (m_activeApplications.find(executableToStart.processName) ==
          m_activeApplications.cend())
      {

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
}

void ExecutionManager::killProcessesForState()
{
  auto allowedApps = m_allowedApplicationForState.find(m_currentState);

  for (auto app = m_activeApplications.cbegin();
       app != m_activeApplications.cend();)
  {
    if (allowedApps == m_allowedApplicationForState.cend() ||
        processToBeKilled(app->first, allowedApps->second))
    {
      kill(app->second, SIGTERM);
      app = m_activeApplications.erase(app);
    }
    else
    {
      app++;
    }
  }
}

bool ExecutionManager::processToBeKilled(const string& app, const std::vector<ProcessName>& allowedApps)
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
    if (drnt->d_name == std::string{"."} ||
        drnt->d_name == std::string{".."})
    {
      continue;
    }

    fileNames.emplace_back(drnt->d_name);

    std::cout << drnt->d_name << std::endl;
  }

  closedir(dp);
  return fileNames;
}

void ExecutionManager::startApplication(const ProcessName& process)
{
  pid_t processId = fork();

  if (!processId)
  {
    // child process
    const auto processPath = corePath
                     + process.applicationName
                     + "/processes/"
                     + process.processName;

    int res = execl(processPath.c_str(), process.processName.c_str(), nullptr);

    if (res)
    {
      throw runtime_error(string{"Error occured creating process: "}
                          + process.processName
                          + " "
                          + strerror(errno));
    }
  } else {
    // parent process
    m_activeApplications.insert({process.processName, processId});
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

  context.getResults().setState(m_currentState);

  context.getResults().setResult(StateError::K_SUCCESS);

  return kj::READY_NOW;
}

::kj::Promise<void>
ExecutionManager::setMachineState(SetMachineStateContext context)
{
  string state = context.getParams().getState().cStr();

  if (!state.empty() && state != m_currentState)
  {
    m_currentState = state;

    killProcessesForState();

    startApplicationsForState();
   
    context.getResults().setResult(StateError::K_SUCCESS);

    std::cout << "Machine state changed successfully to "
              << "\"" 
              << m_currentState << "\""
              << std::endl;
  }
  else
  {
    context.getResults().setResult(StateError::K_INVALID_STATE);

    std::cout << "Invalid machine state received - "
              << "\"" << m_currentState << "\""
              << std::endl;
  }

  return kj::READY_NOW;
}

} // namespace ExecutionManager
