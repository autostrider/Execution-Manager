#include <iostream>
#include <signal.h>
#include <chrono>
#include <thread>

void signal_handler(int n)
{
    std::cout << "=============Bye from proc3============" << std::endl;
    exit(EXIT_SUCCESS);
}

int main()
{
    signal(SIGTERM, signal_handler);
	std::cout << "app2\tproc3\n";
    while (1)
    {
        std::cout << "proc3" <<std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

	return 0;
}
