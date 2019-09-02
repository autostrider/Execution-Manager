#include <iostream>
#include <signal.h>
#include <chrono>
#include <thread>

int main(int argc, char* argv[])
{
    std::cout << "=============Bye from proc1============" << std::endl;
    exit(EXIT_SUCCESS);
}


  std::cout << "argv[0] : " << argv[0] << std::endl;
  std::cout << "argv[1] : " << argv[1] << std::endl;



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
