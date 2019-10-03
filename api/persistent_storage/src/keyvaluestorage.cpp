#include "keyvaluestorage.h"
#include <sstream>

namespace api {

KeyValueStorage::KeyValueStorage(const std::string& database) noexcept(false)
: m_filename(database)
{
  araifstream stream(m_filename);

  if(!stream.is_open())
  {
    throw std::runtime_error("File can't be opened");
  }

  std::stringstream iss;
  for(std::string line; std::getline(stream, line); iss << line) {}

  m_json = nlohmann::json::parse(iss.rdbuf()->str());
}

KeyValueStorage::KeyValueStorage(KeyValueStorage&& other)
: m_filename(std::move(other.m_filename))
{}

KeyValueStorage&
KeyValueStorage::operator=(KeyValueStorage&& right)
{
  m_filename = std::move(right.m_filename);
  return *this;
}

std::vector<std::string>
KeyValueStorage::GetAllKeys() const noexcept(false)
{
  std::vector<std::string> keys;

  std::transform(
    m_json.items().begin(),
    m_json.items().end(),
    std::back_inserter(keys),
    [](const auto& pair)
    {
      return pair.key();
    });

  return keys;
}

bool
KeyValueStorage::HasKey(const std::string& key) const noexcept
{
  return m_json.count(key);
}

KvsType
KeyValueStorage::GetValue(const std::string& key) const noexcept
{
  return KvsType();
}

KvsType
KeyValueStorage::GetDefaultValue(const std::string& key) const noexcept
{
  return KvsType();
}

void
KeyValueStorage::SetValue(const std::string& key, const KvsType& value)
  noexcept(false)
{
  // m_json.emplace(key, value.);
  switch(value.GetType())
  {
    case KvsType::Type::kSInt8:
    case KvsType::Type::kSInt16:
    case KvsType::Type::kSInt32:
    {
      m_json.emplace(key, value.GetSInt());
      break;
    }
    case KvsType::Type::kUInt8:
    case KvsType::Type::kUInt16:
    case KvsType::Type::kUInt32:
    {
      m_json.emplace(key, value.GetUInt());
      break;
    }
    case KvsType::Type::kSInt64:
    {
      m_json.emplace(key, value.GetSInt64());
      break;
    }
    case KvsType::Type::kUInt64:
    {
      m_json.emplace(key, value.GetUInt64());
      break;
    }
    case KvsType::Type::kFloat:
    {
      m_json.emplace(key, value.GetFloat());
      break;
    }
    case KvsType::Type::kDouble:
    {
      m_json.emplace(key, value.GetDouble());
      break;
    }
    case KvsType::Type::kString:
    {
      m_json.emplace(key, value.GetString());
      break;
    }
    case KvsType::Type::kBoolean:
    {
      m_json.emplace(key, value.GetBool());
      break;
    }
    default:
    {
      throw std::logic_error("Logic error");
    }
  }
}

void
KeyValueStorage::RemoveKey(const std::string& key) noexcept
{
  m_json.erase(key);
}

void
KeyValueStorage::RemoveAllKeys() noexcept
{
  m_json.clear();
}

void
KeyValueStorage::SyncToStorage() const noexcept(false)
{
  araofstream stream(m_filename, std::ios::out | std::ios::trunc);

  if(!stream.is_open())
  {
    throw std::runtime_error("File can't be opened");
  }

  stream << m_json.dump();

  stream.flush();
  stream.close();
}

KeyValueStorage::~KeyValueStorage() noexcept
{
  try
  {
    SyncToStorage();
  }
  catch (...)
  {
    LOG << "Exception thrown while synchronization to persistent storage";
  }
}

} // namespace api