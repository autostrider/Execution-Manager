#include <AppStateMachine.hpp>

#include <signal.h>
static void signalHandler(int signo);

static App app;
int main()
{
    if (::signal(SIGINT, signalHandler) == SIG_ERR)
    {
        std::cout << "Error while registering signal\n";
    }

    app.transitToNextState();
    app.transitToNextState();
    app.transitToNextState();
    while(1);
    return 0;
}

static void signalHandler(int signo)
{
    std::cout << "received signal:" << sys_siglist[signo] << "\n";
    app.transitToNextState(App::TerminateApp);
}
