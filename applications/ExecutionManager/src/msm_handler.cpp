#include "msm_handler.hpp"
#include <logger.hpp>

namespace ExecutionManager
{

MsmHandler::MsmHandler()
  : m_msmPId{-1}
{ }

bool MsmHandler::registerMsm(pid_t processId, const std::string& appName)
{
   if ((m_msmPId == -1 ||
      m_msmPId == processId) &&
      !appName.empty())
  {
    m_msmPId = processId;

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

} // namespace ExecutionManager