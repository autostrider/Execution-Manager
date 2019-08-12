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

struct ApplicationManifest;

class ExecutionManager
{
public:
    void start();
private:
    int loadListOfApplications(vector<string>& fileNames);
    vector<ApplicationManifest> processManifests();
    applicationId startApplication(const ApplicationManifest &manifest);

private:
  const static string corePath;
};

} // namespace ExecutionManager

#endif // EXECUTION_MANAGER_HPP
