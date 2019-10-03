#ifndef MSM_HANDLER_HPP
#define MSM_HANDLER_HPP

#include <string>

namespace ExecutionManager
{

class MsmHandler
{
public:
  MsmHandler();
  bool registerMsm(const pid_t& processId, const std::string& appName);
  bool checkMsm(pid_t processId) const;
  pid_t msmPid() const;
private:
  pid_t m_msmPId;
};

} // namespace ExecutionManager

#endif // MSM_HANDLER_HPP