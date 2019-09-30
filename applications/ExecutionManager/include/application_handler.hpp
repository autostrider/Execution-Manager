#ifndef EXECUTION_MANAGER_APPLICATION_HANDLER_HPP
#define EXECUTION_MANAGER_APPLICATION_HANDLER_HPP


#include "manifests.hpp"
#include <i_application_handler.hpp>
#include <i_manifest_reader.hpp>
#include <i_os_interface.hpp>
#include <constants.hpp>

#include <vector>
#include <string>
#include <map>

namespace ExecutionManager
{
using ProcName = std::string;
class ProcessHandler : public IProcessHandler
{
public:
  explicit ProcessHandler(std::unique_ptr<IOsInterface> syscalls,
                              std::string path = APPLICATIONS_PATH);

  pid_t startProcess(const ProcessInfo& process) override;

  void killProcess(pid_t processId) override;

  ~ProcessHandler() override = default;

private:
    /**
   * @brief Builds vector of command line arguments passed to application.
   * @param process: process for certain mode dependent startup config.
   * @return vector of command line arguments for application.
   */
    std::vector<std::string> getArgumentsList(const ProcessInfo& process) const;

    /**
   * @brief Method that converts input std::vector of std::string to
   *        std::vector<char*> to pass as argv in application.
   * @param vectorToConvert: vector that will be converted.
   * @return Vector of char* including `nullptr` that be passed to application.
   */
    std::vector<char*>
    convertToNullTerminatingArgv(
            std::vector<std::string>& vectorToConvert) const;

private:
  const std::string corePath;

  std::unique_ptr<IOsInterface> m_syscalls;
};


class ApplicationHandler : public IApplicationHandler
{
public:
    ApplicationHandler(std::unique_ptr<IProcessHandler> processHandler, std::map<MachineState, std::vector<ProcessInfo>>);

    void filterStates(std::vector<MachineState>& machineManifestStates) override;
    void startApplication(const ProcessInfo& process, std::set<pid_t>&) override;
    void startApplicationsForState(std::set<pid_t>& stateConfirmToBeReceived, MachineState& m_pendingState) override;
    void killProcessesForState(std::set<pid_t>& stateConfirmToBeReceived, MachineState& pendingState) override;

private:
    /**
     * @brief structure that holds application and required processes.
     */
    std::unique_ptr<IProcessHandler> m_ProcessHandler;
    std::map<MachineState, std::vector<ProcessInfo>> m_allowedProcessesForState;
    std::map<ProcName, pid_t> m_activeProcesses;

    bool processToBeKilled (const std::string& app,
                            const std::vector<ProcessInfo>& allowedApps);
};
} // namespace ExecutionManager
#endif //EXECUTION_MANAGER_APPLICATION_HANDLER_HPP
