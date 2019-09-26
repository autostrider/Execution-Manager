#ifndef CONSTANTS
#define CONSTANTS

#include <string>

static const std::string IPC_PROTOCOL{"unix:"};
static const std::string EM_SOCKET_NAME{"/tmp/execution_management"};
static const std::string MSM_SOCKET_NAME{"/tmp/machine_management"};
static const std::string APPLICATIONS_PATH{"./bin/applications/"};

#endif // CONSTANTS