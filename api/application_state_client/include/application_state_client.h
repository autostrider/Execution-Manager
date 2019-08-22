#ifndef APPLICATION_STATE_CLIENT_H
#define APPLICATION_STATE_CLIENT_H

#include <json.hpp>

namespace api {

/**
 * @brief Available application states.
 */
enum class ApplicationState
{
    kInitializing,
    kRunning,
    kShuttingdown
};

/// AppStates serialization & deserialization
NLOHMANN_JSON_SERIALIZE_ENUM(ApplicationState, {
    {ApplicationState::kInitializing, "init"},
    {ApplicationState::kRunning, "running"},
    {ApplicationState::kShuttingdown, "shutdown"}
})

}
#endif // APPLICATION_STATE_CLIENT_H
