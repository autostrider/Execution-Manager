#include "machine_state_manager.hpp"

#include <iostream>


int main(int argc, char* argv[])
{
    MachineStateManager::MachineStateManager manager;

    manager.registerInExecutionManager();

    manager.start();

    while (true)
    {

    }

    return EXIT_SUCCESS;
}
