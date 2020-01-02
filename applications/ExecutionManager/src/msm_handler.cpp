#include "msm_handler.hpp"
#include <logger.hpp>

#include <functional>

namespace ExecutionManager
{

MsmHandler::MsmHandler(std::unique_ptr<IApplicationHandler> appHandler)
  : m_msmPId{-1},
    m_msmObserver{std::move(appHandler)}
{
    auto handler = [&](const std::string& app) { clearMsm(); };
    m_msmObserver.subscribe(handler);
}

bool MsmHandler::registerMsm(pid_t processId, const std::string& appName)
{
   if ((!exists() || checkMsm(processId)) && !appName.empty())
  {
    m_msmPId = processId;
    m_msmObserver.observe(appName);

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
    return processId == m_msmPId;
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
