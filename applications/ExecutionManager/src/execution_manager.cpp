#include "execution_manager.hpp"

#include <json.hpp>
#include <unistd.h>
#include <fstream>
#include <dirent.h>
#include <exception>

#include <iostream>


namespace ExecutionManager {

using std::runtime_error;

const string ExecutionManager::corePath = string{"../applications/AdaptiveApplications/"};

void ExecutionManager::start()
{
    auto manifests = processManifests();

    for (const auto& manf: manifests)
    {
        std::cout << manf.name << std::endl;
        try
        {
            startApplication(manf);
        }
        catch (runtime_error err)
        {
            std::cout << err.what() << std::endl;
            continue;
        }
    }
}

int ExecutionManager::loadListOfApplications(vector<string> &fileNames)
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

    return 0;

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
        std::cout << file << std::endl;
        data.open(file);

        data >> content;
        ApplicationManifest manifest = content;
        res.push_back(manifest);

        data.close();
    }

    return res;
}

applicationId ExecutionManager::startApplication(const ApplicationManifest &manifest)
{
    for (auto name: manifest.processes)
    {
        pid_t processId = fork();

        if (!processId)
        {
            auto processPath = corePath + manifest.name + "/processes/" + name.name;
            std::cout << "process path: " << processPath << std::endl;
            // child process
            int res = execl(processPath.c_str(), name.name.c_str(), nullptr);

            if (res)
            {
                throw runtime_error(string{"Error occured creating process: "} + strerror(errno));
            }
        }
    }

    return manifest.name;


}

} // namespace ExecutionManager
