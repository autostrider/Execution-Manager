#include "keyvaluestorage.h"
#include "kvstype.h"

#include "gtest/gtest.h"

using namespace testing;
using namespace per;

class KeyValueStorageTests : public Test
{
protected:
  std::string key = "key";
  std::string value = "value";
  KvsType kvs = KvsType{value};
};

TEST_F(KeyValueStorageTests, ShouldSuccessfulySaveKeyInStorage)
{
  KeyValueStorage storage{"fl"};

  ASSERT_FALSE(storage.HasKey(key));
  storage.SetValue(key, kvs);
  ASSERT_TRUE(storage.HasKey(key));
}

TEST_F(KeyValueStorageTests, ShouldSuccessfulyGetValueForGivenKey)
{
  KeyValueStorage storage{"fl"};
  storage.SetValue(key, kvs);
  auto actual = storage.GetValue(key);

  ASSERT_EQ(actual.GetString(), value);
  ASSERT_EQ(actual.GetType(), KvsType::Type::kString);
}

TEST_F(KeyValueStorageTests, ShouldSuccessfulyRemoveAllProvidedKeys)
{
  KeyValueStorage storage{"fl"};
  storage.SetValue(key, kvs);
  storage.SetValue("fs", kvs);

  storage.RemoveAllKeys();

  ASSERT_FALSE(storage.HasKey(key));
  ASSERT_FALSE(storage.HasKey("fs"));
}

TEST_F(KeyValueStorageTests, ShouldSuccessfulySyncDataToFileSystem)
{
  const std::string dbPath = "/tmp/temp_db.json";
  KeyValueStorage storage{dbPath};

  storage.SetValue(key, kvs);
  storage.SyncToStorage();

  KeyValueStorage result{dbPath};
  ASSERT_TRUE(result.HasKey(key));
  auto resultValue = result.GetValue(key);
  ASSERT_EQ(resultValue.GetString(), value);
  ASSERT_EQ(resultValue.GetType(), KvsType::Type::kString);
}
