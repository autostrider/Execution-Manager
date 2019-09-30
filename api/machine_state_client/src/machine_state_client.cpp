#include "machine_state_client.h"
#include <constants.hpp>

#include <unistd.h>

#include <thread>
#include <chrono>

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
    m_pid(getpid()),
    m_promise(std::promise<StateError>())
{}

MachineStateClient::~MachineStateClient()
{
  if(m_listenFulfiller.get() == nullptr)
  {
    return;
  }

  const kj::Executor* exec;
  {
    auto lock = m_serverExecutor.lockExclusive();
    lock.wait([&](kj::Maybe<const kj::Executor&> value)
    {
      return value != nullptr;
    });
    exec = &KJ_ASSERT_NONNULL(*lock);
  }

  exec->executeSync([&]()
  {
    m_listenFulfiller->fulfill();
  });
}

MachineStateClient::StateError
MachineStateClient::Register(string appName, std::uint32_t timeout)
{
  auto request = m_clientApplication.registerRequest();
  request.setAppName(appName);
  request.setPid(m_pid);

  auto promise =
    m_timer.timeoutAfter(timeout * kj::MILLISECONDS, request.send());

  m_serverThread = startServer();

  auto result = promise.then([](auto&& res)
      {
        return res.getResult();
      }, exceptionLambda)
    .then([&](auto&& result)
    {
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
      if(_result == StateError::K_SUCCESS)
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
  std::future<StateError> future = m_promise.get_future();

  std::future_status status =
    future.wait_for(std::chrono::milliseconds(timeout));

  if(status == std::future_status::timeout)
  {
    return StateError::K_TIMEOUT;
  }

  auto val = future.get();

  m_promise = std::promise<StateError>();
  return val;
}

kj::Own<kj::Thread>
MachineStateClient::startServer()
{
  std::promise<void> startUpPromise;

  auto sThread = kj::heap<kj::Thread>([&]() noexcept
  {
    auto ioContext = kj::setupAsyncIo();

    capnp::TwoPartyServer server(
      kj::heap<MachineStateServer>(m_promise));
    auto address = ioContext.provider->getNetwork()
        .parseAddress(IPC_PROTOCOL + MSM_SOCKET_NAME)
          .wait(ioContext.waitScope);

    auto listener = address->listen();
    auto listenPromise = server.listen(*listener);

    *m_serverExecutor.lockExclusive() = kj::getCurrentThreadExecutor();

    auto exitPaf = kj::newPromiseAndFulfiller<void>();
    auto exitPromise = listenPromise.exclusiveJoin(kj::mv(exitPaf.promise));

    m_listenFulfiller = kj::mv(exitPaf.fulfiller);

    startUpPromise.set_value();
    exitPromise.wait(ioContext.waitScope);

    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  });

  startUpPromise.get_future().wait();

  return sThread;
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

  m_promise.set_value(result);

  return kj::READY_NOW;
}

} // namespace api
