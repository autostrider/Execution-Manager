#include "mocks.hpp"

IMachineStateManagerMock::IMachineStateManagerMock(std::unique_ptr<api::IStateFactory> factory,
                                                   std::unique_ptr<api::IApplicationStateClientWrapper> appClient,
                                                   std::unique_ptr<api::IMachineStateClientWrapper> machineClient) 
    : MachineStateManager(std::move(factory),
                          std::move(appClient), 
                          std::move(machineClient))
{}