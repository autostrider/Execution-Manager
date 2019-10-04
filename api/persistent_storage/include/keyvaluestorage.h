#ifndef KEY_VALUE_STORAGE_H
#define KEY_VALUE_STORAGE_H

#include "kvstype.h"
#include "araofstream.h"
#include "araifstream.h"
#include <logger.hpp>
#include <json.hpp>

namespace api {

// class KvsType;

class KeyValueStorageBase
{
public:
  KeyValueStorageBase (const KeyValueStorageBase&) = delete;
  KeyValueStorageBase& operator= (const KeyValueStorageBase&) = delete;

  // KeyValueStorageBase (KeyValueStorageBase&& other);
  // KeyValueStorageBase& operator= (KeyValueStorageBase&& right);

  virtual std::vector<std::string> GetAllKeys () const noexcept(false) = 0;
  virtual bool HasKey (const std::string& key) const noexcept = 0;

  virtual KvsType GetValue (const std::string& key) const noexcept = 0;
  virtual KvsType GetDefaultValue (const std::string& key) const noexcept = 0;

  virtual void SetValue (const std::string& key, const KvsType& value)
    noexcept(false) = 0;

  virtual void RemoveKey (const std::string& key) noexcept = 0;
  virtual void RemoveAllKeys () noexcept = 0;

  virtual void SyncToStorage () const noexcept(false) = 0;

  virtual ~KeyValueStorageBase () noexcept = default;

protected:
  KeyValueStorageBase () = default;

};

class KeyValueStorage final : public KeyValueStorageBase
{
public:
  KeyValueStorage (const std::string& database) noexcept(false);

  KeyValueStorage(KeyValueStorage&& other);
  KeyValueStorage& operator=(KeyValueStorage&& right);

  std::vector<std::string> GetAllKeys () const noexcept(false) override;
  bool HasKey (const std::string& key) const noexcept override;

  KvsType GetValue (const std::string& key) const noexcept override;
  KvsType GetDefaultValue (const std::string& key) const noexcept override;

  void SetValue (const std::string& key, const KvsType& value)
    noexcept(false) override;

  void RemoveKey (const std::string& key) noexcept override;
  void RemoveAllKeys () noexcept;

  void SyncToStorage () const noexcept(false) override;

  ~KeyValueStorage () noexcept;

private:

  std::string m_filename;

  nlohmann::json m_json;

  const std::string jsonTypeDenotation{"type"};
  const std::string jsonValueDenotation{"value"};
};

} // namespace api
#endif // KEY_VALUE_STORAGE_H
