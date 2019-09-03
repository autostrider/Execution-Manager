#include <iostream>
#include <signal.h>
#include <chrono>
#include <thread>

void signal_handler(int signal)
{
    std::cout << "=============Bye from proc1============" << std::endl;
    exit(EXIT_SUCCESS);
}


int main(int argc, char* argv[])
{
  signal(SIGTERM, signal_handler);

  for (int i = 0; i < argc; i++)
  {
    std::cout << "argv[" << i << "]=" << std::string{argv[i]} << std::endl;
  }

  std::cout << "app1\tproc1\n";
  while (1)
  {
    std::cout << "proc1" <<std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }

	return 0;
}
