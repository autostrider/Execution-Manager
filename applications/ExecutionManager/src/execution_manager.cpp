#include "execution_manager.hpp"

#include <algorithm>
#include <chrono>
#include <dirent.h>
#include <exception>
#include <fstream>
#include <iostream>
#include <json.hpp>
#include <kj/async-io.h>
#include <signal.h>
#include <thread>
#include <unistd.h>

namespace ExecutionManager
{

namespace {
  const char * applicationStateNames[] =
  {
    "Initializing",
    "Running",
    "Shuttingdown"
  };
} // anonymous namespace

const std::string ExecutionManager::corePath =
  std::string{"./bin/applications/"};

const MachineState ExecutionManager::defaultState {"Starting-up"};

ExecutionManager::ExecutionManager(std::unique_ptr<IManifestReader> reader)
  : m_activeProcesses{},
    m_allowedProcessForState{reader->getStatesSupportedByApplication()},
    m_currentState{defaultState},
    m_machineManifestStates{reader->getMachineStates()},
    m_machineStateClientAppName{""},
    m_stateConfirmToBeReceived{}
{
  std::cout << "[ ExecutionManager ]:\t\tStarted.." << std::endl;

  filterStates();
}

void ExecutionManager::start()
{
  startApplicationsForState();
}

void ExecutionManager::filterStates()
{
  for (auto app = m_activeProcesses.begin(); app != m_activeProcesses.end();)
  {
    if (std::find(m_machineManifestStates.cbegin(),
                  m_machineManifestStates.cend(),
                  app->first) == m_machineManifestStates.cend())
    {
      app = m_activeProcesses.erase(app);
    }
    else
    {
      app++;
    }
  }
}

void ExecutionManager::startApplicationsForState()
{
  const auto& allowedApps = m_allowedProcessForState.find(m_currentState);

  if (allowedApps != m_allowedProcessForState.cend())
  {
    for (const auto& executableToStart: allowedApps->second)
    {
      if (m_activeProcesses.find(executableToStart.processName) ==
          m_activeProcesses.cend())
      {
        try
        {
          startApplication(executableToStart);
        }
        catch (const std::runtime_error& err)
        {
          std::cout << err.what() << std::endl;
        }
      }
    }
  }
}

void ExecutionManager::killProcessesForState()
{
  auto allowedApps = m_allowedProcessForState.find(m_currentState);

  for (auto app = m_activeProcesses.cbegin();
       	    app != m_activeProcesses.cend();)
  {
    if (allowedApps == m_allowedProcessForState.cend() ||
        processToBeKilled(app->first, allowedApps->second))
    {
      kill(app->second, SIGTERM);

      m_stateConfirmToBeReceived.insert(app->second);
      app = m_activeProcesses.erase(app);

    }
    else
    {
      app++;
    }
  }
}

bool ExecutionManager::processToBeKilled(const std::string& app, const std::vector<ProcessInfo>& allowedApps)
{
  auto it = std::find_if(allowedApps.cbegin(),
                     allowedApps.cend(),
                     [&app](auto& listItem)
    { return app == listItem.processName; });

  return (it  == allowedApps.cend());
};

std::vector<std::string>
ExecutionManager::getArgumentsList(const ProcessInfo& process) const
{
  std::vector<std::string> arguments;
  arguments.reserve(process.startOptions.size() + 1);

  // insert app name
  arguments.push_back(process.processName);

  std::transform(process.startOptions.cbegin(),
                 process.startOptions.cend(),
                 std::back_inserter(arguments),
                 [](const StartupOption& option)
  { return option.makeCommandLineOption(); });

  return arguments;
}

std::vector<char *>
ExecutionManager::convertToNullTerminatingArgv(
    std::vector<std::string> &vectorToConvert)
{
  std::vector<char*> outputVector;

  // include terminating sign, that not included in argv
  outputVector.reserve(vectorToConvert.size() + 1);

  for(auto& str: vectorToConvert)
  {
    outputVector.push_back(&str[0]);
  }

  // terminating sign
  outputVector.push_back(nullptr);

  return outputVector;
}

void ExecutionManager::startApplication(const ProcessInfo& process)
{
  pid_t processId = fork();

  if (!processId)
  {
    // child process
    const auto processPath = corePath
                           + process.createRelativePath();

    auto arguments = getArgumentsList(process);
    auto applicationArguments = convertToNullTerminatingArgv(arguments);
    int res = execv(processPath.c_str(), applicationArguments.data());

    if (res)
    {
      throw std::runtime_error(std::string{"[ ExecutionManager ]:\t\tError occured creating process: "}
                          + process.processName
                          + " "
                          + strerror(errno));
    }
  } else {
    // parent process
    if (m_activeProcesses.count(process.processName))
    {
      m_activeProcesses[process.processName] = processId;
    }
    else
    {
      m_activeProcesses.insert({process.processName, {processId}});
    }

    m_stateConfirmToBeReceived.insert(processId);

    m_reportedApplications.insert({processId, process.applicationName});
    std::cout << "[ ExecutionManager ]:\t\tAdaptive aplication \""
              << process.applicationName
              << "\" with pid "
              << processId
              << " started."
              << std::endl;
  }

}

void
ExecutionManager::reportApplicationState(pid_t processId, AppState state)
{
  std::cout << "[ ExecutionManager ]:\t\tState \"" << applicationStateNames[static_cast<uint16_t>(state)]
            << "\" for "
            << m_reportedApplications[processId]
            << " received."
            << std::endl;

  m_stateConfirmToBeReceived.erase(processId);
}

bool
ExecutionManager::registerMachineStateClient(pid_t processId, std::string appName)
{
  if (m_machineStateClientPid == -1 ||
      m_machineStateClientPid == processId)
  {
    m_machineStateClientPid = processId;
    m_machineStateClientAppName = appName;

    std::cout << "[ ExecutionManager ]:\t\tState Machine Client \""
              << m_machineStateClientAppName
              << "\" with pid "
              << m_machineStateClientPid
              << " registered."
              << std::endl;

    return true;
  }

  std::cout << "[ ExecutionManager ]:\t\tState Machine Client \""
            << appName
            << "\" with pid "
            << processId
            << "\" registration failed."
            << std::endl;

  return false;
}

MachineState
ExecutionManager::getMachineState(pid_t processId) const
{
  std::cout << "[ ExecutionManager ]:\t\tgetMachineState request received." << std::endl;

  return m_currentState;
}

bool
ExecutionManager::setMachineState(pid_t processId, std::string state)
{
  auto stateIt = std::find(m_machineManifestStates.cbegin(),
                                 m_machineManifestStates.cend(),
                                 state);

  std::cout << "============================================================================" << std::endl;
  std::cout << "[ ExecutionManager ]:\t\tSTATE TO BE SET : " << state<< std::endl;

  std::set<std::string> allowedAppsSet = getAllowedProcessForState(state);
  std::set<std::string> activeAppsSet = getActiveProcessForCurrentState();

  std::set<std::string> processToBeKilledSet;
  std::set<std::string> processToBeStartedSet;

  std::set_difference(activeAppsSet.begin(), activeAppsSet.end(),
                      allowedAppsSet.begin(), allowedAppsSet.end(),
                      std::inserter(processToBeKilledSet, processToBeKilledSet.begin()));

  std::cout << "[ ExecutionManager ]:\t\tProcesses to be killed: ";
  for (auto process : processToBeKilledSet)
  {
    std::cout << process << ", ";
  }

  std::set_difference(allowedAppsSet.begin(), allowedAppsSet.end(),
                      activeAppsSet.begin(), activeAppsSet.end(),
                      std::inserter(processToBeStartedSet, processToBeStartedSet.begin()));

  std::cout << std::endl << "[ ExecutionManager ]:\t\tProcesses to be started: ";
  for (auto process : processToBeStartedSet)
  {
    std::cout << process <<  ", ";
  }
  std::cout << std::endl;

  allowedAppsSet.clear();
  activeAppsSet.clear();
  processToBeKilledSet.clear();
  processToBeStartedSet.clear();

  if (stateIt != m_machineManifestStates.cend() &&
      processId == m_machineStateClientPid)
  {
    m_currentState = state;

    killProcessesForState();

    startApplicationsForState();

    confirmFromApplication();

    std::cout << "Machine state changed successfully to "
              << "\""
              << m_currentState
              << "\""
              << std::endl;

    std::cout << "============================================================================" << std::endl;

    return true;
  }
  
  std::cout << "[ ExecutionManager ]:\t\tInvalid machine state received - "
            << "\"" << state << "\"."
            << std::endl;

  return false;
}

void ExecutionManager::confirmFromApplication()
{
  std::string listOfPid;
  while(!m_stateConfirmToBeReceived.empty())
  {
    for (auto pid : m_stateConfirmToBeReceived)
    {
      listOfPid += std::to_string(pid) + ", ";
    }
    std::cout << "[ ExecutionManager ]:\t\tWaiting confirm for application with pid == "
              << listOfPid
              << std::endl;

    kj::NEVER_DONE.wait(kj::setupAsyncIo().waitScope);

    listOfPid.clear();      
  }
}

std::set<std::string> ExecutionManager::getAllowedProcessForState(std::string state)
{
  std::set<std::string> allowedProc;

  std::cout << "[ ExecutionManager ]:\t\tProcesses allowed for this state: ";
  auto tmp = m_allowedProcessForState[state];
  
  for (auto id : tmp)
  {
    std::cout << id.processName << ", ";

    allowedProc.insert(id.processName);
  }
  std::cout << std::endl;

  return allowedProc;
}

std::set<std::string> ExecutionManager::getActiveProcessForCurrentState()
{
  std::set<std::string> activeProc;

  std::cout << "[ ExecutionManager ]:\t\tActive process: ";
  for (auto process : m_activeProcesses)
  {
    std::cout << process.first <<  ", ";
    activeProc.insert(process.first);
  }
  std::cout << std::endl;

  return activeProc;
}

} // namespace ExecutionManager
