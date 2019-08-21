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
  : client(path),
    clientApplication(client.getMain<MachineStateManagement>()),
    timer(client.getIoProvider().getTimer())
{}

MachineStateClient::StateError
MachineStateClient::Register(std::string appName, std::uint32_t timeout)
{
  auto request = clientApplication.registerRequest();
  request.setAppName(appName);

  auto promise = timer.timeoutAfter(timeout * kj::MILLISECONDS, request.send());

  auto result = promise.then([](auto&& res)
      {
        return res.getResult();
      }, exceptionLambda).wait(client.getWaitScope());

  return result;
}

MachineStateClient::StateError
MachineStateClient::GetMachineState(std::uint32_t timeout, std::string& state)
{
  auto request = clientApplication.getMachineStateRequest();
  auto promise = timer.timeoutAfter(timeout * kj::MILLISECONDS, request.send());

  auto result = promise.then([&state](auto&& res)
    {
      state = res.getState();

      return res.getResult();
    }, exceptionLambda).wait(client.getWaitScope());

  return result;
}

MachineStateClient::StateError
MachineStateClient::SetMachineState(std::string state, std::uint32_t timeout)
{
  auto request = clientApplication.setMachineStateRequest();
  request.setState(state);

  auto promise = timer.timeoutAfter(timeout * kj::MILLISECONDS, request.send());

  auto result = promise
  .then([](auto&& res)
    {
      return res.getResult();
    }, exceptionLambda).wait(client.getWaitScope());

  return result;
}

} // namespace api