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
    cout << "Hello from SimpleApp" << endl;

    applState->ReportApplicationState(
      ApplicationState::K_INITIALIZING);

    cout << "K_INITIALIZING (0) sent" << endl;

    this_thread::sleep_for(seconds(1));

    applState->ReportApplicationState(
      ApplicationState::K_RUNNING);

    cout << "K_RUNNING (1) sent" << endl;

    this_thread::sleep_for(seconds(1));

    applState->ReportApplicationState(
      ApplicationState::K_SHUTTINGDOWN);

    cout << "K_SHUTTINGDOWN (2) sent" << endl;

    cout << "Bye, bye from SimpleApp" << endl;

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