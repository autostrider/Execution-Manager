#ifndef IAPPLICATION_HANDLER_HPP
#define IAPPLICATION_HANDLER_HPP

#include "manifests.hpp"

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
    virtual pid_t startProcess(const std::string& application) = 0;

    /**
     * @brief Send SIGTERM to application with provided pid
     * @param processId: id of the process
     * @param signal: signal to be sent
     */
    virtual void killProcess(const std::string& processId) = 0;

    virtual ~IApplicationHandler() = default;
};

} // namespace ExecutionManager

#endif // IAPPLICATION_HANDLER_HPP
