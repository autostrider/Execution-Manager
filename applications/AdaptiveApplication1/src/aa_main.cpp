#include <iostream>
#include <signal.h>

void signal_handler(int n)
{
    exit(EXIT_SUCCESS);
}

int main()
{
    signal(SIGTERM, signal_handler);
	std::cout << "app1\tproc1\n";

	return 0;
}
