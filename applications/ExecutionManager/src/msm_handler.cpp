#include "msm_handler.hpp"
#include <logger.hpp>

#include <functional>

namespace ExecutionManager
{

MsmHandler::MsmHandler()
  : m_msmPId{-1}
{
  auto failureHandler = [&] (const std::string& app) { clearMsm(); };  
  m_observer.subscribe(failureHandler);
}

bool MsmHandler::registerMsm(pid_t processId, const std::string& appName)
{
   if ((!exists() || checkMsm(processId)) && !appName.empty())
  {
    m_msmPId = processId;
    m_observer.observe(appName);

    LOG << "State Machine Client \""
        << appName
        << "\" with pid "
        << m_msmPId
        << " registered.";

    return true;
  }

  LOG << "State Machine Client \""
      << appName
      << "\" registration failed"
      << "\" with pid "
      << processId
      << " registration failed.";

  return false;
}

bool MsmHandler::checkMsm(pid_t processId) const
{
  if (exists())
  {
    return processId == m_msmPId;
  }

  return true;
}

pid_t MsmHandler::msmPid() const
{
  return m_msmPId;
}

void MsmHandler::clearMsm()
{
	m_msmPId = -1;
}

bool MsmHandler::exists() const
{
	return m_msmPId != -1;
}

} // namespace ExecutionManager
