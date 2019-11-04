#ifndef IAPPLICATION_HANDLER_HPP
#define IAPPLICATION_HANDLER_HPP

#include "manifests.hpp"

#include <vector>

namespace ExecutionManager {

class IApplicationHandler {
public:
    /**
     * @brief Starts new service.
     * @param application: service file name.
     * @return pid of process
     */
    virtual void startProcess(const std::string& application) = 0;

    /**
     * @brief Send SIGTERM to application with provided pid
     * @param processId: name of service to be killed.
     */
    virtual void killProcess(const std::string& processId) = 0;

    virtual ~IApplicationHandler() = default;
};

} // namespace ExecutionManager

#endif // IAPPLICATION_HANDLER_HPP
