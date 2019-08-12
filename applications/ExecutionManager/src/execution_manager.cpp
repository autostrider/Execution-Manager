#include "execution_manager.hpp"

#include <json.hpp>
#include <unistd.h>
#include <fstream>
#include <dirent.h>

#include <iostream>


namespace ExecutionManager {

void ExecutionManager::start()
{
    auto fileNames = processManifests();

    for (const auto& file: fileNames)
    {
        std::cout << file.name << std::endl;
    }
}

int ExecutionManager::loadFileNamesInDir(string path, vector<string> &fileNames)
{
    DIR* dp;

    if ((dp = opendir(path.c_str())) == nullptr)
    {
        std::cerr << "Error opening directory: " << path << std::endl;
        std::cerr << strerror(errno) << std::endl;

        return errno;
    }

    for (struct dirent *drnt = readdir(dp); drnt != nullptr; drnt = readdir(dp))
    {
        // check for "." and ".." files in directory, we don't need them
        if (!strcmp(drnt->d_name, ".") || !strcmp(drnt->d_name, "..")) continue;

        fileNames.push_back(string{drnt->d_name});
    }

    return 0;

}

vector<ApplicationManifest> ExecutionManager::processManifests()
{
    const static string path = "../apps/manifests/";
    vector<string> fileNames;

    if (loadFileNamesInDir(path, fileNames))
    {
        std::cerr << "Error in processing manifests" << std::endl;
    }

    vector<ApplicationManifest> res;
    json content;
    ifstream data;
    for (auto file: fileNames)
    {
        file = path + file;
        data.open(file);

        data >> content;
        ApplicationManifest manifest = content;
        res.push_back(manifest);

        data.close();
    }

    return res;
}

applicationId ExecutionManager::startApplication(ApplicationManifest &manifest)
{
    return "";
}

} // namespace ExecutionManager
