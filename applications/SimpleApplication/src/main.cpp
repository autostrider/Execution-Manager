#include <iostream>
#include <memory>
#include <chrono>
#include <thread>
#include <application_state_client.h>

using namespace std;
using namespace api;
using namespace chrono;
using ApplicationState = ApplicationStateClient::ApplicationState;

class SimpleApp
{
public:
  SimpleApp() :
    applState(make_unique<ApplicationStateClient>())
  {}
  ~SimpleApp()
  {}
  int start()
  {
    cout << "SimpleApplication: Starting..." << endl;

    cout << "SimpleApplication: Reporting state K_INITIALIZING..." << endl;

    applState->ReportApplicationState(
      ApplicationState::K_INITIALIZING);

    this_thread::sleep_for(seconds(5));

    cout << "SimpleApplication: Reporting state K_RUNNING..." << endl;

    applState->ReportApplicationState(
      ApplicationState::K_RUNNING);

    this_thread::sleep_for(seconds(5));

    cout << "SimpleApplication: Reporting state K_SHUTTINGDOWN..." << endl;

    applState->ReportApplicationState(
      ApplicationState::K_SHUTTINGDOWN);

    cout << "SimpleApplication: Terminating..." << endl;

    return EXIT_SUCCESS;
  }
private:
  unique_ptr<ApplicationStateClient> applState;
};

int main(int argc, char *argv[])
{
  SimpleApp foo;
  return foo.start();
}