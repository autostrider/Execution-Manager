#include <iostream>
#include <thread>
#include <signal.h>
#include <chrono>

void signal_handler(int n)
{
    std::cout << "=============Bye from proc2============" << std::endl;
    exit(EXIT_SUCCESS);
}

int main()
{
    signal(SIGTERM, signal_handler);
	std::cout << "app2\tproc2\n";
    while (1)
    {
        std::cout << "proc2" <<std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    
	return 0;
}
