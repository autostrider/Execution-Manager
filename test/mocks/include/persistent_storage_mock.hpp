#ifndef EXECUTION_MANAGER_PERSISTENT_STORAGE_MOCK_HPP
#define EXECUTION_MANAGER_PERSISTENT_STORAGE_MOCK_HPP

#include <kvstype.h>
#include <keyvaluestorage.h>

#include "gmock/gmock.h"

using namespace per;

class KeyValueStorageMock : public KeyValueStorageBase
{
public:
  KeyValueStorageMock() = default;

  MOCK_CONST_METHOD1(HasKey, bool(const std::string& key));
  MOCK_CONST_METHOD1(GetDefaultValue, KvsType(const std::string& key));
  MOCK_CONST_METHOD1(GetValue, KvsType(const std::string& key));
  MOCK_METHOD2(SetValue, void(const std::string& key, const KvsType& value));
  MOCK_METHOD0(RemoveAllKeys, void());
  MOCK_CONST_METHOD0(SyncToStorage, void()));
};

#endif //EXECUTION_MANAGER_PERSISTENT_STORAGE_MOCK_HPP
