#ifndef EXECUTION_MANAGER_KEYVALUESTORAGE_H
#define EXECUTION_MANAGER_KEYVALUESTORAGE_H

#include <map>
#include <json.hpp>

using nlohmann::json;


namespace per
{

class KvsType;

class KeyValueStorageBase
{
public:
  virtual bool HasKey(const std::string& key) const noexcept = 0;
  virtual KvsType GetValue(const std::string& key) const noexcept = 0;
  virtual void SetValue(const std::string& key, const KvsType& value) noexcept(false) = 0;
  virtual void RemoveAllKeys() noexcept = 0;
  virtual void SyncToStorage() const noexcept(false) = 0;

  virtual ~KeyValueStorageBase() = default;
};

class KeyValueStorage final : public KeyValueStorageBase
{
public:
  KeyValueStorage(const std::string& db);
  KeyValueStorage(const KeyValueStorage&) = delete;
  KeyValueStorage& operator=(const KeyValueStorage&) = delete;

  bool HasKey(const std::string& key) const noexcept override;
  KvsType GetValue(const std::string& key) const noexcept override;
  void SetValue(const std::string& key, const KvsType& value) noexcept(false) override;
  void RemoveAllKeys() noexcept override;
  void SyncToStorage() const noexcept(false) override;

private:
  const std::string dbLocation;
  json  m_data;
};

}

#endif //EXECUTION_MANAGER_KEYVALUESTORAGE_H
