#include "keyvaluestorage.h"
#include <sstream>

namespace api {

KeyValueStorage::KeyValueStorage(const std::string& database) noexcept(false)
: m_filename(database)
{
  araifstream stream(m_filename);

  if(!stream.is_open())
  {
    return;
  }

  std::stringstream iss;
  for(std::string line; std::getline(stream, line); iss << line) {}

  auto rawString = iss.rdbuf()->str();

  try
  {
    m_json = std::move(nlohmann::json::parse(rawString));
  }
  catch(...)
  {
    m_json.clear();
  }
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
  if(!m_json.count(key))
  {
    return KvsType();
  }

  auto kvObject = m_json[key];

  auto type = static_cast<KvsType::Type>(kvObject["type"]);

  switch(type)
  {
    case KvsType::Type::kSInt8:
      return KvsType(kvObject[jsonValueDenotation].get<int8_t>());
    case KvsType::Type::kSInt16:
      return KvsType(kvObject[jsonValueDenotation].get<int16_t>());
    case KvsType::Type::kSInt32:
      return KvsType(kvObject[jsonValueDenotation].get<int32_t>());
    case KvsType::Type::kUInt8:
      return KvsType(kvObject[jsonValueDenotation].get<uint8_t>());
    case KvsType::Type::kUInt16:
      return KvsType(kvObject[jsonValueDenotation].get<uint16_t>());
    case KvsType::Type::kUInt32:
      return KvsType(kvObject[jsonValueDenotation].get<uint32_t>());
    case KvsType::Type::kSInt64:
      return KvsType(kvObject[jsonValueDenotation].get<int64_t>());
    case KvsType::Type::kUInt64:
      return KvsType(kvObject[jsonValueDenotation].get<uint64_t>());
    case KvsType::Type::kFloat:
      return KvsType(kvObject[jsonValueDenotation].get<float>());
    case KvsType::Type::kDouble:
      return KvsType(kvObject[jsonValueDenotation].get<double>());
    case KvsType::Type::kString:
      return KvsType(kvObject[jsonValueDenotation].get<std::string>());
    case KvsType::Type::kBoolean:
      return KvsType(kvObject[jsonValueDenotation].get<bool>());
    default:
    {
      return KvsType();
    }
  }
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
  auto type = value.GetType();
  nlohmann::json kvObject = {{jsonTypeDenotation, static_cast<uint16_t>(type)}};

  switch(type)
  {
    case KvsType::Type::kSInt8:
    case KvsType::Type::kSInt16:
    case KvsType::Type::kSInt32:
    {
      kvObject.emplace(jsonValueDenotation, value.GetSInt());
      break;
    }
    case KvsType::Type::kUInt8:
    case KvsType::Type::kUInt16:
    case KvsType::Type::kUInt32:
    {
      kvObject.emplace(jsonValueDenotation, value.GetUInt());
      break;
    }
    case KvsType::Type::kSInt64:
    {
      kvObject.emplace(jsonValueDenotation, value.GetSInt64());
      break;
    }
    case KvsType::Type::kUInt64:
    {
      kvObject.emplace(jsonValueDenotation, value.GetUInt64());
      break;
    }
    case KvsType::Type::kFloat:
    {
      kvObject.emplace(jsonValueDenotation, value.GetFloat());
      break;
    }
    case KvsType::Type::kDouble:
    {
      kvObject.emplace(jsonValueDenotation, value.GetDouble());
      break;
    }
    case KvsType::Type::kString:
    {
      kvObject.emplace(jsonValueDenotation, value.GetString());
      break;
    }
    case KvsType::Type::kBoolean:
    {
      kvObject.emplace(jsonValueDenotation, value.GetBool());
      break;
    }
    case KvsType::Type::kBinary:
    {
      kvObject.emplace(jsonValueDenotation, value.GetBool());
      break;
    }
    default:
    {
      throw std::logic_error("Logic error");
    }
  }

  m_json[key] = std::move(kvObject);
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