#ifndef IAPPLICATION_HANDLER_HPP
#define IAPPLICATION_HANDLER_HPP

#include <string>
#include <vector>

class IAppplicationHandler
{
public:
  /**
   * @brief Starts new process with params and returns its' pid
   * @param application: path to executable
   * @param arguments: list of arguments (without nullptr)
   * @return pid of process
   */
  virtual pid_t startApplication(const std::string& application,
                         const std::vector<std::string>& arguments) = 0;

  /**
   * @brief Send `signal` to application with provided pid
   * @param processId: id of the process
   * @param signal: signal to be sent
   */
  virtual void killApplication(pid_t processId, int signal) = 0;

  virtual ~IAppplicationHandler() = default;
};

#endif // IAPPLICATION_HANDLER_HPP
