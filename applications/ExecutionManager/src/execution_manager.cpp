#include "execution_manager.hpp"

#include <json.hpp>
#include <unistd.h>
#include <fstream>
#include <dirent.h>
#include <exception>

#include <iostream>


namespace ExecutionManager {

const string ExecutionManager::corePath = string{"../applications/AdaptiveApplications/"};

void ExecutionManager::start()
{
    auto manifests = processManifests();

    for (const auto& manf: manifests)
    {
        std::cout << manf.name << std::endl;
        startApplication(manf);
    }
}

int ExecutionManager::loadListOfApplications(vector<string> &fileNames)
{
    DIR* dp;

    if ((dp = opendir(corePath.c_str())) == nullptr)
    {
        std::cerr << "Error opening directory: " << corePath << std::endl;
        std::cerr << strerror(errno) << std::endl;

        return errno;
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

    if (loadListOfApplications(applicationNames))
    {
        std::cerr << "Error in processing manifests" << std::endl;
    }

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
                std::cerr << "Error occured creating process\n";
                std::cerr << strerror(errno) << std::endl << res << std::endl;
                return "";
            }
        }
    }

    return manifest.name;


}

} // namespace ExecutionManager
