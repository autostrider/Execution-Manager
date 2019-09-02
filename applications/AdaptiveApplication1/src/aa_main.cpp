#include <AppStateMachine.hpp>

#include <signal.h>
static void signalHandler(int signo);

int main()
{
    if (::signal(SIGINT, signalHandler) == SIG_ERR)
    {
        std::cout << "Error while registering signal\n";
    }

    App::getInstance().transitToNextState();
    while(1);
    return 0;
}

static void signalHandler(int signo)
{
    std::cout << "received signal:" << sys_siglist[signo] << "\n";
    App::getInstance().terminate();
    App::getInstance().transitToNextState();
}
