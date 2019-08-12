#ifndef EXECUTION_MANAGER_HPP
#define EXECUTION_MANAGER_HPP

#include "manifests.hpp"

#include <vector>
#include <string>
#include <cstdint>

namespace ExecutionManager
{

using std::vector;
using std::string;
using applicationId = std::string;

class ExecutionManager
{
public:
    void start();
private:
    int loadFileNamesInDir(string path, vector<string>& fileNames);
    vector<ApplicationManifest> processManifests();
    applicationId startApplication(ApplicationManifest &manifest);
};

} // namespace ExecutionManager

#endif // EXECUTION_MANAGER_HPP
