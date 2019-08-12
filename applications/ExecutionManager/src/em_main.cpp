#include <iostream>
#include "em_in.hpp"
#include <execution_management_api.hpp>


int main(int argc, char **argv)
{
  std::cout << "Alice id = " << hello() << std::endl;

  return EXIT_SUCCESS;
}