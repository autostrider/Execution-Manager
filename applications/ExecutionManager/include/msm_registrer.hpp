#ifndef MSMREGISTRER
#define MSMREGISTRER

#include <string>

namespace ExecutionManager
{

class MsmRegister
{
public:
  MsmRegister();
  bool registerMsm(const pid_t& processId, const std::string& appName);
  bool checkMsm(pid_t processId) const;
  pid_t msmPid() const;
private:
  pid_t m_msmPId;
  std::string m_msmAppName;
};

} // namespace ExecutionManager

#endif // MSMREGISTRER