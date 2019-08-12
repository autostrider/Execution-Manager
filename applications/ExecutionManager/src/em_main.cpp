#include "em_in.hpp"
#include "execution_manager.hpp"

#include <iostream>

int main(int argc, char **argv)
{
  std::cout << foo() << std::endl;

  ExecutionManager::ExecutionManager manager;
  manager.start();

  return EXIT_SUCCESS;
}
