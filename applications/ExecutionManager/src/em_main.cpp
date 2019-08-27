#include "execution_manager.hpp"

#include <iostream>

int main(int argc, char **argv)
{
  ExecutionManager::ExecutionManager manager;

  return manager.start();
}
