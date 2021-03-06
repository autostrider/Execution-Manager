#ifndef MANIFEST_READER_MOCK_HPP
#define MANIFEST_READER_MOCK_HPP

#include <i_manifest_reader.hpp>

#include "gmock/gmock.h"

namespace api
{

class ManifestReaderMock : public IManifestReader
{
public:
  MOCK_METHOD0(getStatesSupportedByApplication, std::map<MachineState, std::set<ProcName>>());

  MOCK_METHOD0(getMachineStates, std::vector<MachineState>());
};

} // namespace api
#endif // MANIFEST_READER_MOCK_HPP