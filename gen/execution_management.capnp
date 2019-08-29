@0x91a1015104aef424;

using import "application_state_management.capnp".ApplicationStateManagement;
using import "machine_state_management.capnp".MachineStateManagement;

interface ExecutionManagement extends(ApplicationStateManagement, 
                                      MachineStateManagement) 
{}