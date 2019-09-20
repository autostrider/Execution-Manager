#include "machine_state_manager.hpp"
#include <constants.hpp>

namespace MSM {

using api::MachineStateClient;
using ApplicationState = api::ApplicationStateClient::ApplicationState;
using StateError = api::MachineStateClient::StateError;

MachineStateManager::MachineStateManager(std::unique_ptr<api::IStateFactory> factory,
                                         std::unique_ptr<api::IApplicationStateClientWrapper> client) :
        machineStateClient(std::make_unique<MachineStateClient>(SOCKET_NAME)),
        m_factory{std::move(factory)},
        m_currentState{nullptr},
        m_appClient{std::move(client)}
{
}

void MachineStateManager::init()
{
    m_currentState = m_factory->createInit(*this);
    m_currentState->enter();
}

void MachineStateManager::run()
{
    transitToNextState(
                std::bind(&api::IStateFactory::createRun, m_factory.get(), std::placeholders::_1)
                );
}

void MachineStateManager::terminate()
{
    transitToNextState(
                std::bind(&api::IStateFactory::createShutDown, m_factory.get(), std::placeholders::_1)
                );
}

void MachineStateManager::transitToNextState(api::IAdaptiveApp::FactoryFunc nextState)
{
    m_currentState->leave();
    m_currentState = nextState(*this);
    m_currentState->enter();
}

void MachineStateManager::reportApplicationState(ApplicationState state)
{
    m_appClient->ReportApplicationState(state);
}

void MachineStateManager::setMachineState(std::string state, int timeout)
{
  machineStateClient->SetMachineState(state, timeout);
}

StateError MachineStateManager::registerMsm(const std::string& applicationName, uint timeout)
{
  return machineStateClient->Register(applicationName.c_str(), timeout);
}
//  m_machineStateClient.waitForConfirm(defaultTimeout);

/*
int32_t MachineStateManager::start()
{
  cout << "Machine State Manager started.." << endl;

  const char* applicationName = "Machine state manager";
  constexpr int defaulTimeout = 300000;

  StateError result =
    machineStateClient->Register(applicationName, defaulTimeout);

  if(StateError::K_SUCCESS == result)
  {
    cout << "Successful registration as a MSM" << endl;
  }
  else
  {
    cerr << "Unsuccessful registration as a MSM.\nTerminating.." << endl;
    return EXIT_FAILURE;
  }

  {
    string cli;
    while(cli != "exit")
    {
      std::getline(std::cin, cli);

      if(cli == "get")
      {
        string state;
        StateError result =
          machineStateClient->GetMachineState(defaulTimeout, state);

        if(StateError::K_SUCCESS == result)
        {
          cout << "Current machine state: " << state << endl;
        }
        else if(StateError::K_TIMEOUT == result)
        {
          cout << "Get timeout" << endl;
        }
        else
        {
          cout << "Failed" << endl;
        }
      }
      else if(cli.find("set ") != string::npos)
      {
        string state = cli.substr(cli.find(' ')+1, cli.size() - cli.find(' '));

        StateError result =
          machineStateClient->SetMachineState(state, defaulTimeout);

        if(StateError::K_SUCCESS == result)
        {
          cout << "Machine state changed" << endl;
        }
        else
        {
          cout << "Machine state change failed" << endl;
        }
      }
    }
  }

  return EXIT_SUCCESS;
}
*/
} // namespace MSM
