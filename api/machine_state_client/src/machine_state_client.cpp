#include "machine_state_client.h"

namespace api {

namespace {
  const auto exceptionLambda = [](kj::Exception&& exception)
    {
      switch(exception.getType())
        {
        case kj::Exception::Type::OVERLOADED:
        case kj::Exception::Type::DISCONNECTED:
          return MachineStateClient::StateError::K_TIMEOUT;
        default:
          return MachineStateClient::StateError::K_INVALID_REQUEST;
        }
    };
}

MachineStateClient::MachineStateClient(std::string path)
  : m_client(path),
    m_clientApplication(m_client.getMain<MachineStateManagement>()),
    m_timer(m_client.getIoProvider().getTimer()),
    m_pid(getpid())
{}

MachineStateClient::StateError
MachineStateClient::Register(std::string appName, std::uint32_t timeout)
{
  auto request = m_clientApplication.registerRequest();
  request.setAppName(appName);
  request.setPid(m_pid);

  auto promise = m_timer.timeoutAfter(timeout * kj::MILLISECONDS, request.send());

  auto result = promise.then([](auto&& res)
      {
        return res.getResult();
      }, exceptionLambda).wait(m_client.getWaitScope());

  return result;
}

MachineStateClient::StateError
MachineStateClient::GetMachineState(std::uint32_t timeout, std::string& state)
{
  auto request = m_clientApplication.getMachineStateRequest();
  auto promise = m_timer.timeoutAfter(timeout * kj::MILLISECONDS, request.send());

  request.setPid(m_pid);

  auto result = promise.then([&state](auto&& res)
    {
      state = res.getState();

      return res.getResult();
    }, exceptionLambda).wait(m_client.getWaitScope());

  return result;
}

MachineStateClient::StateError
MachineStateClient::SetMachineState(std::string state, std::uint32_t timeout)
{
  auto request = m_clientApplication.setMachineStateRequest();
  request.setState(state);
  request.setPid(m_pid);

  auto promise = m_timer.timeoutAfter(timeout * kj::MILLISECONDS, request.send());

  auto result = promise
  .then([](auto&& res)
    {
      return res.getResult();
    }, exceptionLambda).wait(m_client.getWaitScope());

  return result;
}

} // namespace api