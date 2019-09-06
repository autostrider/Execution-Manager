#include <iostream>
#include <signal.h>
#include <chrono>
#include <thread>

#include "machine_state_manager.hpp"

static void signalHandler(int signo);
static std::atomic<bool> isTerminating{false};

int main(int argc, char **argv)
{
  if (::signal(SIGINT, signalHandler) == SIG_ERR)
  {
    std::cout << "Error while registering signal\n";
  }
  
  MachineStateManager::MachineStateManager msm(isTerminating);

  while (true)
  {
    msm.transitToNextState();
    std::this_thread::sleep_for(std::chrono::seconds(10));
  }
  
  return 0;
}

static void signalHandler(int signo)
{
    std::cout << "received signal:" << sys_siglist[signo] << "\n";
    isTerminating = true;
}