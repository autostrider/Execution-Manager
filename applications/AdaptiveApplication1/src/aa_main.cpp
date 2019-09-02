#include <iostream>
#include <signal.h>
#include <chrono>
#include <thread>

void signal_handler(int n)
{
    std::cout << "AdaptiveApplication1: Terminating proc1..." << std::endl;
    exit(EXIT_SUCCESS);
}

int main()
{
    signal(SIGTERM, signal_handler);
	std::cout << "AdaptiveApplication1: Starting proc1..." << std::endl;
    while (1)
    {
        std::cout << "AdaptiveApplication1: proc1 is running" <<std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

	return 0;
}
