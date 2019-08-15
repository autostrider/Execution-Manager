#ifndef EXECUTION_MANAGER_HPP
#define EXECUTION_MANAGER_HPP

#include "manifests.hpp"

#include <vector>
#include <string>
#include <cstdint>
#include <unistd.h>
#include <map>

namespace ExecutionManager
{

using std::vector;
using std::map;
using std::pair;
using std::string;
using applicationId = std::string;

struct ApplicationManifest;

class ExecutionManager
{
public:
    /**
     * @brief Main method of Execution manager.
     */
    int start();
private:
    /**
     * typedef for holding process name and app it belongs to
     */
    struct ProcessName
    {
        string applicationName;
        string processName;
    };
    /**
     * @brief Loads all adaptive applications from corePath.
     * @param fileNames: output parameter, where all the applications
     *                   names are stored.
     */
    void loadListOfApplications(vector<string>& fileNames);

    /**
     * @brief processManifests - loads manifests from corePath.
     * @return vector of Application manifest for available
     *          applications respectively.
     */
    vector<ApplicationManifest> processManifests();
    /**
     * @brief Starts given application and stores information
     *        about it in activeApplications.
     * @param manifest: Application manifest of application to start.
     */
    void startApplication(const ProcessName &manifest);

    void startMachineStateManager();
    void confirmMachineState(MachineStates state);
    void loadApplicationsForState();
    void killProcessesForState();

private:
    /// \brief Hardcoded path to folder with adaptive applications.
    const static string corePath;

    /// \brief structure that holds application and required processes.
    map<string, pid_t> activeApplications;

    /// \brief structure for application that can run in certain state
    /// vector consists of applicationId (name) and string param - executable name
    map<MachineStates, vector<ProcessName>> applications;

    /// \brief Current machine state.
    MachineStates currentState = MachineStates::kInit;
};

} // namespace ExecutionManager

#endif // EXECUTION_MANAGER_HPP
