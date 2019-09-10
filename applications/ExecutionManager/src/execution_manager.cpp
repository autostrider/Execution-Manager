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
using std::runtime_error;
using std::string;

namespace {
  const char * applicationStateNames[] =
  {
    "Initializing",
    "Running",
    "Shuttingdown"
  };
} // anonymous namespace

const string ExecutionManager::corePath =
  string{"./bin/applications/"};

const MachineState ExecutionManager::defaultState {"Starting-up"};

ExecutionManager::ExecutionManager(std::unique_ptr<IManifestReader> reader)
  : m_activeProcesses({}),
    m_allowedApplicationForState(std::move(reader->getStatesSupportedByApplication())),
    m_currentState (defaultState),
    m_machineManifestStates({reader->getMachineStates()}),
    m_machineStateClientAppName(""),
    m_stateConfirmToBeReceived({})
{
  std::cout << "[ ExecutionManager ]:\t\tStarted.." << std::endl;

  filterStates();

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
  const auto& allowedApps = m_allowedApplicationForState.find(m_currentState);

  if (allowedApps != m_allowedApplicationForState.cend())
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

  for (auto app = m_activeProcesses.cbegin();
       	    app != m_activeProcesses.cend();)
  {
    if (allowedApps == m_allowedApplicationForState.cend() ||
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

bool ExecutionManager::processToBeKilled(const string& app, const std::vector<ProcessInfo>& allowedApps)
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
      throw runtime_error(string{"[ ExecutionManager ]:\t\tError occured creating process: "}
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

::kj::Promise<void>
ExecutionManager::reportApplicationState(ReportApplicationStateContext context)
{
  ApplicationState state = context.getParams().getState();
  pid_t applicationPid = context.getParams().getPid();

  std::cout << "[ ExecutionManager ]:\t\tState \"" << applicationStateNames[static_cast<uint16_t>(state)]
            << "\" for "
            << m_reportedApplications[applicationPid]
            << " received."
            << std::endl;

  m_stateConfirmToBeReceived.erase(applicationPid);


  return kj::READY_NOW;
}

::kj::Promise<void>
ExecutionManager::register_(RegisterContext context)
{
  string newMachineClientAppName = context.getParams().getAppName();
  pid_t newMachineClientAppPid = context.getParams().getPid();

  if (m_machineStateClientPid == -1 ||
      m_machineStateClientPid == newMachineClientAppPid)
  {
    m_machineStateClientPid = newMachineClientAppPid;
    m_machineStateClientAppName = newMachineClientAppName;

    context.getResults().setResult(StateError::K_SUCCESS);

    std::cout << "[ ExecutionManager ]:\t\tState Machine Client \""
              << m_machineStateClientAppName
              << "\" with pid "
              << newMachineClientAppPid
              << " registered."
              << std::endl;
  }
  else
  {
    context.getResults().setResult(StateError::K_INVALID_REQUEST);

    std::cout << "[ ExecutionManager ]:\t\tState Machine Client \""
              << m_machineStateClientAppName
              << "\" with pid "
              << m_machineStateClientPid
              << "\" registration failed."
              << std::endl;
  }

  return kj::READY_NOW;
}

::kj::Promise<void>
ExecutionManager::getMachineState(GetMachineStateContext context)
{
  std::cout << "[ ExecutionManager ]:\t\tgetMachineState request received." << std::endl;

  context.getResults().setState(m_currentState);

  context.getResults().setResult(StateError::K_SUCCESS);

  return kj::READY_NOW;
}

::kj::Promise<void>
ExecutionManager::setMachineState(SetMachineStateContext context)
{
  string state = context.getParams().getState().cStr();
  pid_t machineStateClientPid = context.getParams().getPid();

  std::cout << "============================================================================" << std::endl;
  std::cout << "[ ExecutionManager ]:\t\tSTATE TO BE SET : " << state<< std::endl;
  std::vector<ProcessInfo> tmp = m_allowedApplicationForState[state];
  std::string res;

  std::set<std::string> allowedAppsSet;

  for (auto id : tmp)
  {
    res += id.processName + ", ";

    allowedAppsSet.insert(id.processName);
  }

  std::string res2;
  std::set<std::string> activeAppsSet;

  for (auto process : m_activeProcesses)
  {
    res2 += process.first + ", ";
    activeAppsSet.insert(process.first);
  }

  std::set<std::string> processeToBeKilled;
  std::set_difference(activeAppsSet.begin(), activeAppsSet.end(),
                      allowedAppsSet.begin(), allowedAppsSet.end(),
                      std::inserter(processeToBeKilled, processeToBeKilled.begin()));


  std::set<std::string> processeToBeStarted;
  std::set_difference(allowedAppsSet.begin(), allowedAppsSet.end(),
                      activeAppsSet.begin(), activeAppsSet.end(),
                      std::inserter(processeToBeStarted, processeToBeStarted.begin()));

  std::string res3;
  for (auto process : processeToBeKilled)
  {
    res3 += process + ", ";
  }

  std::string res4;
  for (auto process : processeToBeStarted)
  {
    res4 += process + ", ";
  }

  std::cout << "[ ExecutionManager ]:\t\tApplications allowed for this state: "  << res << std::endl;
  std::cout << "[ ExecutionManager ]:\t\tActive process: "  << res2 << std::endl;
  std::cout << "[ ExecutionManager ]:\t\tProcess to be killed: "  << res3 << std::endl;
  std::cout << "[ ExecutionManager ]:\t\tProcess to be started: "  << res4 << std::endl;

  res.clear();
  res2.clear();

  if (!state.empty() &&
      state != m_currentState &&
      machineStateClientPid == m_machineStateClientPid)
  {
    m_currentState = state;

    killProcessesForState();

    startApplicationsForState();

    confirmFromApplication();

    std::cout << "============================================================================" << std::endl;
  }
  else
  {
    context.getResults().setResult(StateError::K_INVALID_STATE);

    std::cout << "[ ExecutionManager ]:\t\tInvalid machine state received - "
              << "\"" << m_currentState << "\"."
              << std::endl;

    return kj::READY_NOW;
  }

  context.getResults().setResult(StateError::K_SUCCESS);
  return kj::READY_NOW;
}

void ExecutionManager::confirmFromApplication()
{
  std::string tmp1;
  while(!m_stateConfirmToBeReceived.empty())
  {
    for (auto pid : m_stateConfirmToBeReceived)
    {
      tmp1 += std::to_string(pid) + ", ";
    }
    std::cout << "[ ExecutionManager ]:\t\tWaiting confirm for application with pid == " << tmp1 << std::endl;

    kj::NEVER_DONE.wait(kj::setupAsyncIo().waitScope);

    tmp1.clear();      
  }
}

} // namespace ExecutionManager
