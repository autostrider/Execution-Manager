#ifndef IAPPLICATION_HANDLER_HPP
#define IAPPLICATION_HANDLER_HPP


namespace api
{

class IApplicationHandler
{
public:
    virtual bool startProcess(const std::string& service) = 0;

    virtual bool killProcess(const std::string& service) = 0;

    virtual bool isActiveProcess(const std::string& service) = 0;

    virtual ~IApplicationHandler() = default;
};

} // namespace api

#endif // IAPPLICATION_HANDLER_HPP
