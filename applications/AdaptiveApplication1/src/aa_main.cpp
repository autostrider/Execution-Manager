#include <AppStateMachine.hpp>

#include <signal.h>

static void siginthandler(int signo);

int main()
{
    if (::signal(SIGINT, siginthandler) == SIG_ERR)
    {
        std::cout << "Error while registering signal\n";
    }
    std::cout << "app1\tproc1\n";

    App::start();
    App::dispatch(Running());
    while(1);
    return 0;
}

static void siginthandler(int signo)
{
    std::cout << "received signal:" << sys_siglist[signo] << "\n";
    App::dispatch(Terminating());
    ::exit(EXIT_SUCCESS);
}
