#include "execution_manager.hpp"


namespace ExecutionManager
{
using std::cout;
using std::endl;
using std::cerr;
using std::map;
using std::runtime_error;
using std::string;
using std::vector;

const string ExecutionManager::corePath =
  string{"applications/"};

int32_t ExecutionManager::start()
{
  auto manifests = processManifests();

  for (const auto &manf : manifests)
  {
    cout << manf.name << endl;
    try
    {
      startApplication(manf);
    }
    catch (const runtime_error& err)
    {
      cerr << err.what() << endl;
      continue;
    }
  }

  try
  {
    capnp::EzRpcServer server(kj::heap<ExecutionManager>(),
                              "unix:/tmp/execution_management");
    auto &waitScope = server.getWaitScope();

    cout << "Execution Manager started.." << endl;

    kj::NEVER_DONE.wait(waitScope);
  }
  catch (const kj::Exception &e)
  {
    cerr << e.getDescription().cStr() << endl;
  }

  return EXIT_SUCCESS;
}

void ExecutionManager::loadListOfApplications(vector<string> &fileNames)
{
  DIR *dp = nullptr;

  if ((dp = opendir(corePath.c_str())) == nullptr)
  {
    throw runtime_error(string{"Error opening directory: "} + corePath + " " + strerror(errno));
  }

  for (struct dirent *drnt = readdir(dp); drnt != nullptr; drnt = readdir(dp))
  {
    // check for "." and ".." files in directory, we don't need them
    if (!strcmp(drnt->d_name, ".") || !strcmp(drnt->d_name, ".."))
      continue;

    fileNames.emplace_back(drnt->d_name);

    cout << "Adaptive application directory \""
         << drnt->d_name << "\" added" << endl;
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
  for (auto file : applicationNames)
  {
    file = corePath + file + "/manifest.json";
    data.open(file);

    data >> content;
    ApplicationManifest manifest = content;
    res.push_back(manifest);

    data.close();
  }

  return res;
}

void ExecutionManager::startApplication(const ApplicationManifest &manifest)
{
  vector<pid_t> applicationProcessIds;
  for (const auto &process : manifest.processes)
  {
    pid_t processId = fork();

    if (!processId)
    {
      // child process
      auto processPath = corePath + manifest.name + "/processes/" + process.name;
      int res = execl(processPath.c_str(), process.name.c_str(), nullptr);

      if (res)
      {
        throw runtime_error(string{"Error occured creating process: "} + process.name + " " + strerror(errno));
      }
      // add process to application processes.
      applicationProcessIds.push_back(processId);
    }
  }

  activeApplications.insert(std::pair<string, vector<pid_t>>{manifest.name, applicationProcessIds});
}

::kj::Promise<void>
ExecutionManager::reportApplicationState(ReportApplicationStateContext context)
{
  ApplicationState state = context.getParams().getState();

  cout << "State #" << static_cast<uint16_t>(state)
       << " received"
       << endl;

  return kj::READY_NOW;
}

::kj::Promise<void>
ExecutionManager::register_(RegisterContext context)
{
  string newMachineClient = context.getParams().getAppName();

  if (machineStateClientAppName.empty() ||
      machineStateClientAppName == newMachineClient)
  {
    machineStateClientAppName = newMachineClient;
    context.getResults().setResult(StateError::K_SUCCESS);

    cout << "State Machine Client \""
         << machineStateClientAppName
         << "\" registered" << endl;
  }
  else
  {
    context.getResults().setResult(StateError::K_INVALID_REQUEST);

    cout << "State Machine Client \""
         << machineStateClientAppName
         << "\" registration failed" << endl;
  }

  return kj::READY_NOW;
}

::kj::Promise<void>
ExecutionManager::getMachineState(GetMachineStateContext context)
{
  cout << "getMachineState request received" << endl;

  context.getResults().setState(machineState);

  context.getResults().setResult(StateError::K_SUCCESS);

  return kj::READY_NOW;
}

::kj::Promise<void>
ExecutionManager::setMachineState(SetMachineStateContext context)
{
  std::string state = context.getParams().getState().cStr();

  if (!state.empty() && state != machineState)
  {
    machineState = state;

    context.getResults().setResult(StateError::K_SUCCESS);

    cout << "Machine state changed successfully to "
         << "\"" << machineState << "\"" << endl;
  }
  else
  {
    context.getResults().setResult(StateError::K_INVALID_STATE);

    cout << "Invalid machine state received - "
         << "\"" << machineState << "\"" << endl;
  }

  return kj::READY_NOW;
}

} // namespace ExecutionManager