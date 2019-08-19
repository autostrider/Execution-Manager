#include <iostream>
#include "execution_manager.hpp"

int main(int argc, char **argv)
{
  ExecutionManager::ExecutionManager em;

  return em.start();
}