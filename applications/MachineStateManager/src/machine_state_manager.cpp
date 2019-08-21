#include "machine_state_manager.hpp"

namespace MachineStateManager {

using api::MachineStateClient;
using std::string;
using std::cout;
using std::cin;
using std::cerr;
using std::endl;

MachineStateManager::MachineStateManager()
  : machineStateClient(std::make_unique<MachineStateClient>
                        ("unix:/tmp/execution_management"))
{}

int32_t MachineStateManager::start()
{
  cout << "Machine State Manager started.." << endl;

  const char* applicationName = "Machine state manager";
  constexpr int defaulTimeout = 3000;

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
        } else{cout << "Failed" << endl;}
      }
      else if(cli.find("set ") != string::npos)
      {
        string state = cli.substr(cli.find(' ')+1, cli.size() - cli.find(' '));

        StateError result =
          machineStateClient->SetMachineState(state, defaulTimeout);

        if(StateError::K_SUCCESS == result)
        {
          cout << "Machine state changed" << endl;
        }else{cout << "Failed" << endl;}
      }
    }
  }

  return EXIT_SUCCESS;
}


} // namespace MachineStateManager