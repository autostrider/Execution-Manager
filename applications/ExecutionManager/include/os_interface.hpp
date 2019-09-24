#ifndef EXECUTION_MANAGER_OSINTERFACE_HPP
#define EXECUTION_MANAGER_OSINTERFACE_HPP

#include <i_os_interface.hpp>

namespace ExecutionManager
{

class OsInterface : public IOsInterface
{
public:
  pid_t fork() override;
  int execv(const char* path, char* argv[]) override;
  int kill(pid_t procId, int signal) override;
  int waitpid(pid_t procId, int *status, int options) override;

    ~OsInterface() override = default;
};

} // namespace ExecutionManager

#endif //EXECUTION_MANAGER_OSINTERFACE_HPP
