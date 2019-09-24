#ifndef CONSTANTS
#define CONSTANTS

#include <string>
#include <chrono>

static const std::string SOCKET_NAME{"unix:/tmp/execution_management"};
static const std::string APPLICATIONS_PATH{"./bin/applications/"};

constexpr std::uint32_t DEFAULT_RESPONSE_TIMEOUT{3000000};
constexpr std::uint32_t NO_TIMEOUT{0};
constexpr std::chrono::seconds FIVE_SECONDS{5};

static const std::string AA_STATE_INIT{"Initializing"};
static const std::string AA_STATE_RUNNING{"Running"};
static const std::string AA_STATE_SHUTDOWN{"Shutdown"};

static const std::string MACHINE_STATE_STARTUP{"Starting-up"};
static const std::string MACHINE_STATE_RUNNING{"Running"};
static const std::string MACHINE_STATE_LIVING{"Living"};
static const std::string MACHINE_STATE_SHUTTINGDOWN{"Shuttingdown"};



#endif // CONSTANTS