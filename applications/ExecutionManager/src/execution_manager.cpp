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

const string ExecutionManager::corePath = string{"../applications/AdaptiveApplications/"};

const vector<MachineStates> ExecutionManager::transition =
            {MachineStates::kInit, MachineStates::kRunning, MachineStates::kShutdown};

int ExecutionManager::start()
{
    processManifests();

    for (const auto& state: transition)
    {
        std::cout << "————————————————————————————————————————————————————————\n";
        currentState = state;

        killProcessesForState();
        switch (state)
        {
        case  MachineStates::kInit:
            std::cout <<"init\n";
            break;
        case MachineStates::kRunning:
            std::cout << "run\n";
            break;
        case MachineStates::kShutdown:
            std::cout << "shhutdown\n";
            break;
        case MachineStates::kRestart:
            std::cout << "restart\n";
            break;
        }

        startApplicationsForState();

        std::this_thread::sleep_for(std::chrono::seconds{2});
    }

    return EXIT_SUCCESS;
}

void ExecutionManager::startApplicationsForState()
{
    vector<ProcessName> processesToStart;
    std::copy_if(applications[currentState].begin(),
                 applications[currentState].end(),
                 std::back_inserter(processesToStart),
                 [=](auto item)
                 { return activeApplications.find(item.processName) == activeApplications.end(); });

    for (const auto& executableToStart: processesToStart)
    {
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
    const vector<ProcessName> allowedApps = applications[currentState];

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

void ExecutionManager::loadListOfApplications(vector<string> &fileNames)
{
    DIR* dp;

    if ((dp = opendir(corePath.c_str())) == nullptr)
    {
        throw runtime_error(string{"Error opening directory: "} + strerror(errno));
    }

    for (struct dirent *drnt = readdir(dp); drnt != nullptr; drnt = readdir(dp))
    {
        // check for "." and ".." files in directory, we don't need them
        if (!strcmp(drnt->d_name, ".") || !strcmp(drnt->d_name, "..")) continue;

        fileNames.emplace_back(drnt->d_name);

        std::cout << drnt->d_name << std::endl;
    }

    closedir(dp);
}

void ExecutionManager::processManifests()
{
    vector<string> applicationNames;

    loadListOfApplications(applicationNames);

    json content;
    ifstream data;
    for (auto file: applicationNames)
    {
        file = corePath + file + "/manifest.json";
        data.open(file);

        data >> content;
        ApplicationManifest manifest = content;

        for (const auto& process: manifest.processes)
        {
            for (const auto& procState: process.startMachineStates)
            {
                applications[procState].push_back({manifest.name, process.name});
            }
        }

        data.close();
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
            throw runtime_error(string{"Error occured creating process: "} + strerror(errno));
        }
    } else {
        // parent process
        activeApplications.insert({manifest.processName, processId});
    }

}

} // namespace ExecutionManager
