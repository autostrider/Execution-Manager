#include "AppStateMachine.hpp"

#include <signal.h>


static void siginthandler(int signo);

using fsm_handle = App;

int main()
{
    if (::signal(SIGINT, siginthandler) == SIG_ERR)
    {
        std::cout << "Error while registering signal\n";
        return 1;
    }
    std::cout << "app1\tproc1\n";

    fsm_handle::start();
   // fsm_handle::dispatch(kInitializing());
    fsm_handle::dispatch(kRunning());
    fsm_handle::dispatch(kTerminating());
    return 0;
}

static void siginthandler(int signo)
{
    std::cout << "received signal:" << sys_siglist[signo];
    ::exit(1);
}


