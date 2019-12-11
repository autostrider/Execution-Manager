#ifndef EXECUTION_MANAGER_APPLICATION_HANDLER_HPP
#define EXECUTION_MANAGER_APPLICATION_HANDLER_HPP


#include "manifests.hpp"
#include <i_application_handler.hpp>
#include <i_os_interface.hpp>
#include <constants.hpp>

#include <vector>
#include <string>

namespace ExecutionManager
{

class ApplicationHandler : public IApplicationHandler
{
public:
  explicit ApplicationHandler(std::unique_ptr<IOsInterface> syscalls,
                              std::string path = APPLICATIONS_PATH);

  void startProcess(const std::string& serviceName) override;

  void killProcess(const std::string& serviceName) override;

  bool isActiveProcess(const std::string& serviceName) override;

  ~ApplicationHandler() override = default;

private:
  void
  execProcess(const std::string& processName, const std::string& action) const;

  std::vector<char*>
  convertToNullTerminatingArgv(
          std::vector<std::string>& vectorToConvert) const;

private:
  const std::string corePath;

  std::unique_ptr<IOsInterface> m_syscalls;
};

} // namespace ExecutionManager
#endif //EXECUTION_MANAGER_APPLICATION_HANDLER_HPP
