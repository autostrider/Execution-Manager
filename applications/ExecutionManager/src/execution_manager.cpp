#include "execution_manager.hpp"

#include <json.hpp>
#include <fstream>
#include <dirent.h>
#include <exception>
#include <thread>

#include "machine_state_manager.hpp"

#include <iostream>


namespace ExecutionManager {

using std::runtime_error;

const string ExecutionManager::corePath = string{"../applications/AdaptiveApplications/"};

int ExecutionManager::start()
{
    auto manifests = processManifests();

    for (const auto& manif: manifests)
    {
        for (const auto& process: manif.processes)
        {
            for (const auto& procState: process.startMachineStates)
            {
                applications[procState].push_back({manif.name, process.name});
            }
        }
    }

    std::cout << "Here\n";
    for (auto state = MachineStateManager::MachineStateManager::transition.begin();
         state != MachineStateManager::MachineStateManager::transition.end();
         state++)
    {
        std::cout << "------------------------------------------------------\n";
        currentState = *state;

        switch (currentState)
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

        loadApplicationsForState();

        std::this_thread::sleep_for(std::chrono::seconds{2});
    }

    return EXIT_SUCCESS;
}

void ExecutionManager::loadApplicationsForState()
{
    vector<ProcessName> processesToStart;
    std::copy_if(applications[currentState].begin(),
                 applications[currentState].end(),
                 std::back_inserter(processesToStart),
                 [this](auto item) { return this->activeApplications.find(item.processName) == this->activeApplications.end(); });

    for (const auto& executableToStart: processesToStart)
    {
        try
        {
            std::cout << executableToStart.processName << " " << executableToStart.applicationName << std::endl;
            startApplication(executableToStart);
        }
        catch (runtime_error err)
        {
            std::cout << err.what() << std::endl;
            continue;
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

        fileNames.push_back(string{drnt->d_name});

        std::cout << drnt->d_name << std::endl;
    }

    closedir(dp);
}

vector<ApplicationManifest> ExecutionManager::processManifests()
{
    vector<string> applicationNames;

    loadListOfApplications(applicationNames);

    vector<ApplicationManifest> res;
    json content;
    ifstream data;
    for (auto file: applicationNames)
    {
        file = corePath + file + "/manifest.json";
        data.open(file);

        data >> content;
        ApplicationManifest manifest = content;
        res.push_back(manifest);

        data.close();
    }

    std::cout << "size: " << res.size() << std::endl;

    return res;
}

void ExecutionManager::startApplication(const ProcessName &manifest)
{
    vector<pid_t> applicationProcessIds;

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
            // add process to application processes.
            applicationProcessIds.push_back(processId);
        }

    activeApplications[manifest.processName].push_back(processId);
}

} // namespace ExecutionManager
