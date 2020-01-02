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

  MOCK_METHOD(bool, HasKey, (const std::string& key), (const, noexcept));
  MOCK_METHOD(KvsType, GetDefaultValue, (const std::string& key),
              (const, noexcept));
  MOCK_METHOD(KvsType, GetValue, (const std::string& key), (const, noexcept));
  MOCK_METHOD(void, SetValue, (const std::string& key, const KvsType& value),
              (noexcept(false)));
  MOCK_METHOD(void, RemoveAllKeys, (), (noexcept));
  MOCK_METHOD(void, SyncToStorage, (), (const, noexcept(false)));
};

#endif //EXECUTION_MANAGER_PERSISTENT_STORAGE_MOCK_HPP
