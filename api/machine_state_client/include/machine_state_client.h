#ifndef MACHINE_STATE_CLIENT_H
#define MACHINE_STATE_CLIENT_H

namespace api {

/**
 * @brief Enum with available machine states.
 */
enum class MachineStates
{
    kInit,
    kRunning,
    kShutdown,
    kRestart = -1
};

}
#endif // MACHINE_STATE_CLIENT_H
