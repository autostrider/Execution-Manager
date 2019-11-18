#ifndef MANIFEST_READER_MOCK_HPP
#define MANIFEST_READER_MOCK_HPP

#include "i_manifest_reader.hpp"

#include "gmock/gmock.h"

namespace ExecutionManager
{

class ManifestReaderMock : public IManifestReader
{
public:
  MOCK_METHOD((std::map<MachineState, ProcessesContainer>),
    getStatesSupportedByApplication, (), (override));

  MOCK_METHOD(std::vector<MachineState>, getMachineStates, ());
};

} // namespace ExecutionManager
#endif // MANIFEST_READER_MOCK_HPP