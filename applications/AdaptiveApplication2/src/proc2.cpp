#include <iostream>
#include <thread>
#include <signal.h>

void signal_handler(int n)
{
    exit(EXIT_SUCCESS);
}

int main()
{
    signal(SIGTERM, signal_handler);
	while (1) {
		std::cout << "app2\tproc2\n";
		std::this_thread::sleep_for(std::chrono::milliseconds{500});
	}
	return 0;
}
