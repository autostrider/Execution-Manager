#include "msm_registrer.hpp"
#include <logger.hpp>

namespace ExecutionManager
{

MsmRegister::MsmRegister()
  : m_msmPId{-1},
    m_msmAppName{""}
{ }

bool MsmRegister::registerMsm(const pid_t& processId, const std::string& appName)
{
   if ((m_msmPId == -1 ||
      m_msmPId == processId) &&
      !appName.empty())
  {
    m_msmPId = processId;
    m_msmAppName = appName;

    LOG << "State Machine Client \""
        << m_msmAppName
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

bool MsmRegister::checkMsm(pid_t processId) const
{
  return processId == m_msmPId;
}

pid_t MsmRegister::msmPid() const
{
  return m_msmPId;
}

} // namespace ExecutionManager