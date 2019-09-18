#ifndef EXECUTION_MANAGER_APPLICATION_HANDER_HPP
#define EXECUTION_MANAGER_APPLICATION_HANDER_HPP

#include "iapplication_handler.hpp"
#include "manifests.hpp"

namespace ExecutionManager
{

class ApplicationHandler : public IApplicationHandler
{
public:
  explicit ApplicationHandler(std::string path = "./bin/applications/");

  pid_t startProcess(const ProcessInfo &process) override;

  void killApplication(pid_t processId, int signal) override;

  ~ApplicationHandler() override = default;

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
            std::vector<std::string> &vectorToConvert) const;

private:
    const std::string corePath;
};

} // namespace ExecutionManager
#endif //EXECUTION_MANAGER_APPLICATION_HANDER_HPP
