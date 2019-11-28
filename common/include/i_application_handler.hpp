#ifndef IAPPLICATION_HANDLER_HPP
#define IAPPLICATION_HANDLER_HPP

#include "manifests.hpp"

#include <vector>

namespace ExecutionManager {

class IApplicationHandler {
public:
    virtual void startProcess(const std::string& service) = 0;

    virtual void killProcess(const std::string& service) = 0;

    virtual bool isActiveProcess(const std::string& service) = 0;

    virtual ~IApplicationHandler() = default;
};

} // namespace ExecutionManager

#endif // IAPPLICATION_HANDLER_HPP
