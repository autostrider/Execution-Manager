#ifndef IAPPLICATION_HANDLER_HPP
#define IAPPLICATION_HANDLER_HPP

#include "manifests.hpp"

#include <string>
#include <vector>

namespace ExecutionManager {

class IApplicationHandler {
public:
    /**
     * @brief Starts new process with params and returns its' pid
     * @param application: path to executable
     * @param arguments: list of arguments (without nullptr)
     * @return pid of process
     */
    virtual pid_t startProcess(const ProcessInfo &application) = 0;

    /**
     * @brief Send `signal` to application with provided pid
     * @param processId: id of the process
     * @param signal: signal to be sent
     */
    virtual void killApplication(pid_t processId, int signal) = 0;

    virtual ~IApplicationHandler() = default;
};

} // namespace ExecutionManager

#endif // IAPPLICATION_HANDLER_HPP
