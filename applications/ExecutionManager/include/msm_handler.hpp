#ifndef MSM_HANDLER_HPP
#define MSM_HANDLER_HPP

#include "msm_observer.hpp"

#include <string>

namespace ExecutionManager
{

class MsmHandler
{
public:
  MsmHandler();
  bool registerMsm(pid_t processId, const std::string& appName);
  bool checkMsm(pid_t processId) const;
  pid_t msmPid() const;
	bool exists() const; 
	void clearMsm();
private:
  pid_t m_msmPId;
  MsmObserver m_observer;
};

} // namespace ExecutionManager

#endif // MSM_HANDLER_HPP
