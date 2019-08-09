#ifndef APPLICATION_STATE_CLIENT_H
#define APPLICATION_STATE_CLIENT_H


namespace ExecutionManager {

/**
 * @brief Available application states.
 */
enum class ApplicationState
{
    kInitializing,
    kRunning,
    kShuttingdown
};

}
#endif // APPLICATION_STATE_CLIENT_H
