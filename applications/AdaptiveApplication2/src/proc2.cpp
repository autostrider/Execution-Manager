#include <iostream>
#include <thread>
#include <signal.h>
#include <chrono>

void signal_handler(int n)
{
    std::cout << "AdaptiveApplication2: Terminating proc2..." << std::endl;
    exit(EXIT_SUCCESS);
}

int main()
{
    signal(SIGTERM, signal_handler);
	std::cout << "AdaptiveApplication2: Starting proc2..." << std::endl;
    while (1)
    {
        std::cout << "AdaptiveApplication2: proc2 is running" <<std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    
	return 0;
}
