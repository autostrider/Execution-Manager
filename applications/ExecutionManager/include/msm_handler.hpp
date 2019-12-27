#ifndef MSM_HANDLER_HPP
#define MSM_HANDLER_HPP

#include "app_observer.hpp"
#include "os_interface.hpp"
#include <string>
#include <memory>

namespace ExecutionManager
{

class MsmHandler
{
public:
  MsmHandler(std::unique_ptr<IOsInterface> os = std::make_unique<OsInterface>());
  bool registerMsm(pid_t processId, const std::string& appName);
  bool checkMsm(pid_t processId) const;
  pid_t msmPid() const;
  bool exists() const;
  void clearMsm();
private:
  pid_t m_msmPId;
  AppObserver m_msmObserver;
};

} // namespace ExecutionManager

#endif // MSM_HANDLER_HPP
