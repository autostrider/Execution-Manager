#include "machine_state_client.h"

using std::string;

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

MachineStateClient::MachineStateClient(string path)
:
    m_client(path),
    m_clientApplication(m_client.getMain<MachineStateManagement>()),
    m_timer(m_client.getIoProvider().getTimer()),
    m_pid(getpid())
{}

MachineStateClient::~MachineStateClient()
{
  if(m_serverExecutor != nullptr)
  {
    KJ_ASSERT_NONNULL(m_serverExecutor).executeSync([&]()
    {
      m_listenFulfiller->fulfill();
    });
  }
}

MachineStateClient::StateError
MachineStateClient::Register(string appName, std::uint32_t timeout)
{
  auto request = m_clientApplication.registerRequest();
  request.setAppName(appName);
  request.setPid(m_pid);

  auto promise =
    m_timer.timeoutAfter(timeout * kj::MILLISECONDS, request.send());

  auto result = promise.then([](auto&& res)
      {
        return res.getResult();
      }, exceptionLambda)
    .then([&](auto&& result)
    {
      startServer();
      return result;
    }).wait(m_client.getWaitScope());

  return result;
}

MachineStateClient::StateError
MachineStateClient::GetMachineState(std::uint32_t timeout, string& state)
{
  auto request = m_clientApplication.getMachineStateRequest();
  request.setPid(m_pid);

  auto promise =
    m_timer.timeoutAfter(timeout * kj::MILLISECONDS, request.send());

  auto result = promise.then([&state](auto&& res)
    {
      state = res.getState();

      return res.getResult();
    }, exceptionLambda).wait(m_client.getWaitScope());

  return result;
}

MachineStateClient::StateError
MachineStateClient::SetMachineState(string state, std::uint32_t timeout)
{
  auto request = m_clientApplication.setMachineStateRequest();
  request.setState(state);
  request.setPid(m_pid);

  auto promise =
    m_timer.timeoutAfter(timeout * kj::MILLISECONDS, request.send());

  auto result = promise.then([&](auto&& res)
    {
      auto _result = res.getResult();
      if(res.getResult() == StateError::K_SUCCESS)
      {
        return waitForConfirm(timeout);
      }
      else
      {
        return _result;
      }
    }, exceptionLambda).wait(m_client.getWaitScope());

  return result;
}

MachineStateClient::StateError
MachineStateClient::waitForConfirm(std::uint32_t timeout)
{
  m_promise = std::promise<StateError>();
  std::future<StateError> future = m_promise.get_future();

  std::future_status status =
    future.wait_for(std::chrono::milliseconds(timeout));

  if(status == std::future_status::timeout)
  {
    return StateError::K_TIMEOUT;
  }

  return future.get();
}

void
MachineStateClient::startServer()
{
  if(m_serverExecutor != nullptr)
  {
    return;
  }

  m_serverThread = m_client.getIoProvider().newPipeThread(
    [&](kj::AsyncIoProvider& ioProvider,
        auto&,
        kj::WaitScope& waitScope)
  {
    capnp::TwoPartyServer server(
      kj::heap<MachineStateServer>(m_promise));
    auto address = ioProvider.getNetwork()
        .parseAddress(std::string{"unix:/tmp/machine_management"})
          .wait(waitScope);

    auto listener = address->listen();
    auto listenPromise = server.listen(*listener);

    auto exitPaf = kj::newPromiseAndFulfiller<void>();
    auto exitPromise = listenPromise.exclusiveJoin(kj::mv(exitPaf.promise));

    m_listenFulfiller = kj::mv(exitPaf.fulfiller);
    m_serverExecutor = kj::getCurrentThreadExecutor();

    exitPromise.wait(waitScope);
  });
}

MachineStateServer::MachineStateServer
  (std::promise<StateError>& promise)
: m_promise(promise)
{}

::kj::Promise<void>
MachineStateServer::confirmStateTransition
(ConfirmStateTransitionContext context)
{
  auto result = context.getParams().getResult();
  std::cout << "confirmStateTransition" << static_cast<uint16_t>(result) << std::endl;

  m_promise.set_value(result);

  return kj::READY_NOW;
}

} // namespace api