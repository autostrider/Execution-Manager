#include "execution_manager.hpp"

#include <json.hpp>
#include <fstream>
#include <dirent.h>
#include <exception>
#include <thread>
#include <signal.h>


#include <iostream>


namespace ExecutionManager {

using std::runtime_error;

const MachineState ExecutionManager::corePath = std::string{"../applications/AdaptiveApplications/"};

const std::vector<MachineState> ExecutionManager::transition =
            {"init", "running", "shutdown"};

int ExecutionManager::start()
{
    processManifests();

    for (const auto& state: transition)
    {
        std::cout << "————————————————————————————————————————————————————————\n";
        currentState = state;

        killProcessesForState();
        std::cout << state << std::endl;

        startApplicationsForState();

        std::this_thread::sleep_for(std::chrono::seconds{2});
    }

    return EXIT_SUCCESS;
}

void ExecutionManager::startApplicationsForState()
{
    const auto& allowedApps = allowedApplicationForState[currentState];

    for (const auto& executableToStart: allowedApps)
    {
        if (activeApplications.find(executableToStart.processName) != activeApplications.end())
            continue;
        try
        {
            startApplication(executableToStart);
        }
        catch (runtime_error err)
        {
            std::cout << err.what() << std::endl;
            continue;
        }
    }
}

void ExecutionManager::killProcessesForState()
{
    const auto& allowedApps = allowedApplicationForState[currentState];

    for (auto app = activeApplications.begin(); app != activeApplications.end();)
    {
        if (std::find_if(allowedApps.begin(),
                         allowedApps.end(),
                         [&app](auto& allowedApp)
                         { return app->first == allowedApp.processName; }) == allowedApps.end())
        {
            kill(app->second, SIGTERM);
            app = activeApplications.erase(app);
        } else {
            app++;
        }
    }
}

std::vector<std::string> ExecutionManager::loadListOfApplications()
{
    DIR* dp = nullptr;
    std::vector<std::string> fileNames;

    if ((dp = opendir(corePath.c_str())) == nullptr)
    {
      throw runtime_error(std::string{"Error opening directory: "}
                            + corePath
                            + " "
                            + strerror(errno));
    }

    for (struct dirent *drnt = readdir(dp); drnt != nullptr; drnt = readdir(dp))
    {
        // check for "." and ".." files in directory, we don't need them
        if (!strcmp(drnt->d_name, ".") || !strcmp(drnt->d_name, "..")) continue;

        fileNames.emplace_back(drnt->d_name);

        std::cout << drnt->d_name << std::endl;
    }

    closedir(dp);
    return fileNames;
}

void ExecutionManager::processManifests()
{
    const auto& applicationNames = loadListOfApplications();

    json content;
    for (auto file: applicationNames)
    {
        file = corePath + file + "/manifest.json";
        ifstream data{file};

        data >> content;
        ApplicationManifest manifest = content;

        for (const auto& process: manifest.manifest.processes)
        {
            for (const auto& procState: process.modeDependentStartupConf)
            {
                allowedApplicationForState[procState.modes].push_back({manifest.manifest.manifestId, process.name});
            }
        }

    }
}

void ExecutionManager::startApplication(const ProcessName &manifest)
{
    pid_t processId = fork();

    if (!processId)
    {
        // child process
        auto processPath = corePath + manifest.applicationName + "/processes/" + manifest.processName;

        int res = execl(processPath.c_str(), manifest.processName.c_str(), nullptr);

        if (res)
        {
            throw runtime_error(string{"Error occured creating process: "}
                                + manifest.processName
                                + " "
                                + strerror(errno));
        }
    } else {
        // parent process
        activeApplications.insert({manifest.processName, processId});
    }

}

} // namespace ExecutionManager
