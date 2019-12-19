#ifndef CONSTANTS
#define CONSTANTS

#include <application_state_client.h>

#include <functional>
#include <csignal>
#include <string>
#include <chrono>
#include <map>

static const std::string IPC_PROTOCOL{"unix:"};
static const std::string MSM_STATE_PROVIDER{"/tmp/data.json"};
static const std::string EM_SOCKET_NAME{"/tmp/execution_management"};
static const std::string MSM_STATES_SERVER{"/tmp/state_setter"};
static const std::string MSM_SOCKET_NAME{"/tmp/machine_management"};
static const std::string MSM_TEST{"/tmp/msc_test"};
static const std::string APPLICATIONS_PATH{"./bin/applications/"};

constexpr std::uint32_t DEFAULT_RESPONSE_TIMEOUT{30000000};
constexpr std::uint32_t NO_TIMEOUT{0};
constexpr std::chrono::seconds FIVE_SECONDS{5};

static const std::string AA_STATE_INIT{"Initializing"};
static const std::string AA_STATE_RUNNING{"Running"};
static const std::string AA_STATE_SHUTDOWN{"Shutdown"};
static const std::string AA_STATE_SUSPEND{"Suspend"};

static const std::string MACHINE_STATE_STARTUP{"Starting-up"};
static const std::string MACHINE_STATE_RUNNING{"Running"};
static const std::string MACHINE_STATE_LIVING{"Living"};
static const std::string MACHINE_STATE_SUSPEND{"Suspend"};
static const std::string MACHINE_STATE_SHUTTINGDOWN{"Shuttingdown"};

static const std::string COMPONENT_STATE_ON{"kOn"};
static const std::string COMPONENT_STATE_OFF{"kOff"};

using StateHandler = std::function<void()>;
using ProcName = std::string;


static const std::string SYSTEMCTL{"systemctl"};
static const std::string SERVICE_EXTENSION{".service"};
static const std::string USER{"--user"};
static const std::string SYSTEMCTL_START{"start"};
static const std::string SYSTEMCTL_STOP{"stop"};
#endif // CONSTANTS
