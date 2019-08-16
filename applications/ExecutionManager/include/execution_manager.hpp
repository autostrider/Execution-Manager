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
using std::string;
using applicationId = std::string;

struct ApplicationManifest;

class ExecutionManager
{
public:
    /**
     * @brief Main method of Execution manager.
     */
    void start();
private:
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
    void startApplication(const ApplicationManifest &manifest);

private:
    /// \brief Hardcoded path to folder with adaptive applications.
    const static string corePath;

    /// \brief structure that holds application and required processes.
    map<applicationId, vector<pid_t>> activeApplications;
};

} // namespace ExecutionManager

#endif // EXECUTION_MANAGER_HPP
