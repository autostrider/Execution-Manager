#ifndef IAPPLICATION_HANDLER_HPP
#define IAPPLICATION_HANDLER_HPP

#include "manifests.hpp"

#include <vector>
#include <set>

namespace ExecutionManager {

class IProcessHandler {
public:
    /**
     * @brief Starts new process with params and returns its' pid
     * @param application: path to executable
     * @param arguments: list of arguments (without nullptr)
     * @return pid of process
     */
    virtual pid_t startProcess(const ProcessInfo& application) = 0;

    /**
     * @brief Send SIGTERM to application with provided pid
     * @param processId: id of the process
     * @param signal: signal to be sent
     */
    virtual void killProcess(pid_t processId) = 0;

    virtual ~IProcessHandler() = default;
};

class IApplicationHandler
{
public:
    virtual void filterStates(std::vector<MachineState>&) = 0;
    virtual void startApplication(const ProcessInfo& process, std::set<pid_t>&) = 0;
    virtual void startApplicationsForState(std::set<pid_t>& stateConfirmToBeReceived, MachineState&) = 0;
    virtual void killProcessesForState(std::set<pid_t>&, MachineState&) = 0;
    virtual ~IApplicationHandler() = default;

};
} // namespace ExecutionManager

#endif // IAPPLICATION_HANDLER_HPP
