#include "keyvaluestorage.h"
#include "kvstype.h"
#include <logger.hpp>

#include <map>
#include <fstream>

namespace per
{

void to_json(json& jsonObject, const KvsType& kvsType)
{
  jsonObject = json{{"value", kvsType.GetString()}};
}

void from_json(const json& jsonObject, KvsType& kvsType)
{
  std::string result;
  jsonObject.at("value").get_to(result);
  kvsType = KvsType(result);
}


bool KeyValueStorage::HasKey(const std::string &key) const noexcept
{
  return m_data.find(key) != m_data.cend();
}

void KeyValueStorage::SetValue(const std::string& key,
                               const KvsType& value) noexcept(false)
{
  m_data[key] = value;
}

KvsType
KeyValueStorage::GetValue(const std::string& key) const noexcept
{
    return m_data[key];
}

void KeyValueStorage::RemoveAllKeys() noexcept
{
  m_data.clear();
}

void KeyValueStorage::SyncToStorage() const noexcept(false)
{
  std::ofstream file{dbLocation};
  file << m_data;
}

KeyValueStorage::KeyValueStorage(const std::string& db)
        : dbLocation{db}
{
  std::ifstream checker{dbLocation};
  if (checker.good() && json::accept(checker))
  {
    checker.clear();
    checker.seekg(0, std::ios::beg);
    checker >> m_data;
  }
}

}