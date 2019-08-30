#include <Application.hpp>

#include <signal.h>
static void signalHandler(int signo);

int main()
{
    if (::signal(SIGINT, signalHandler) == SIG_ERR)
    {
        std::cout << "Error while registering signal\n";
    }

    Application::getInstance().transitToNextState();
    Application::getInstance().transitToNextState();
    Application::getInstance().transitToNextState();
    while(1);
    return 0;
}

static void signalHandler(int signo)
{
    std::cout << "received signal:" << sys_siglist[signo] << "\n";
    Application::getInstance().transitToNextState(App::TerminateApp);
}
