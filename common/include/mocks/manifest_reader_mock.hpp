#ifndef MANIFESTREADERMOCK
#define MANIFESTREADERMOCK

#include "i_manifest_reader.hpp"

#include "gmock/gmock.h"

using namespace ExecutionManager;

class ManifestReaderMock : public IManifestReader
{
public:
  MOCK_METHOD((std::map<MachineState, std::vector<ProcessInfo>>),
    getStatesSupportedByApplication, (), (override));

  MOCK_METHOD(std::vector<MachineState>, getMachineStates, ());
};

#endif // MANIFESTREADERMOCK