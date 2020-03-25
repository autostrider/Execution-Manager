#ifndef IAPPLICATION_HANDLER_HPP
#define IAPPLICATION_HANDLER_HPP


namespace ExecutionManager {

class IApplicationHandler
{
public:
    virtual bool startProcess(const std::string& service) = 0;

    virtual bool killProcess(const std::string& service) = 0;

    virtual bool isActiveProcess(const std::string& service) = 0;

    virtual ~IApplicationHandler() = default;
};

} // namespace ExecutionManager

#endif // IAPPLICATION_HANDLER_HPP
