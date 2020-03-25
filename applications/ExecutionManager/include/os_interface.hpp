#ifndef EXECUTION_MANAGER_OSINTERFACE_HPP
#define EXECUTION_MANAGER_OSINTERFACE_HPP

#include <i_os_interface.hpp>

namespace ExecutionManager
{

class OsInterface : public api::IOsInterface
{
public:
  OsInterface(){}
  pid_t fork() override;
  int execvp(const char *file, char **argv) override;
  FILE *popen(const char* command, const char* type) override;
  int pclose(FILE *stream) override;
  size_t fread(void* ptr, size_t size, size_t count, FILE *stream) override;
  ~OsInterface() override = default;
};

} // namespace ExecutionManager

#endif //EXECUTION_MANAGER_OSINTERFACE_HPP
