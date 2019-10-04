#include "KvsType.h"
#include <stdexcept>
#include <cstring>

namespace api {

KvsType::KvsType()
: m_type(Type::kNotSet), m_status(Status::kNotFound), _array()
{}

KvsType::KvsType(bool value)
: m_type(Type::kBoolean), m_status(Status::kSuccess), _bool(value)
{}

KvsType::KvsType(int8_t value)
: m_type(Type::kSInt8), m_status(Status::kSuccess), _integer(value)
{}

KvsType::KvsType(int16_t value)
: m_type(Type::kSInt16), m_status(Status::kSuccess), _integer(value)
{}

KvsType::KvsType(int32_t value)
: m_type(Type::kSInt32), m_status(Status::kSuccess), _integer(value)
{}

KvsType::KvsType(int64_t value)
: m_type(Type::kSInt64), m_status(Status::kSuccess), _integer(value)
{}

KvsType::KvsType(uint8_t value)
: m_type(Type::kUInt8), m_status(Status::kSuccess), _integer(value)
{}

KvsType::KvsType(uint16_t value)
: m_type(Type::kUInt16), m_status(Status::kSuccess), _integer(value)
{}

KvsType::KvsType(uint32_t value)
: m_type(Type::kUInt32), m_status(Status::kSuccess), _integer(value)
{}

KvsType::KvsType(uint64_t value)
: m_type(Type::kUInt64), m_status(Status::kSuccess), _integer(value)
{}

KvsType::KvsType(float value)
: m_type(Type::kFloat), m_status(Status::kSuccess), _float(value)
{}

KvsType::KvsType(double value)
: m_type(Type::kDouble), m_status(Status::kSuccess), _double(value)
{}

KvsType::KvsType(std::string value)
: m_type(Type::kString), m_status(Status::kSuccess), _str(value)
{}

KvsType::KvsType(void* data, std::size_t len)
: m_type(Type::kBinary), m_status(Status::kSuccess)
{
  _data.len = len;
  _data.data = (void*)new uint8_t[len];

  std::memcpy(_data.data, data, len);
}

KvsType::~KvsType()
{
  if(m_type == Type::kBinary && _data.len)
  {
    delete [] (uint8_t*)_data.data;
  }
}

KvsType::KvsType(KvsType&& other)
{
  *this = std::move(other);
}

KvsType& KvsType::operator=(KvsType&& right)
{
  if (this == &right)
  {
    return *this;
  }

  auto type = right.GetType();

  switch(type)
  {
    case KvsType::Type::kSInt8:
    case KvsType::Type::kSInt16:
    case KvsType::Type::kSInt32:
    case KvsType::Type::kSInt64:
    case KvsType::Type::kUInt8:
    case KvsType::Type::kUInt16:
    case KvsType::Type::kUInt32:
    case KvsType::Type::kUInt64:
      _integer = right._integer;
      break;
    case Type::kFloat:
      _float = right._float;
      break;
    case Type::kDouble:
      _double = right._double;
      break;
    case Type::kString:
      _str = std::move(right._str);
      break;
    case Type::kBoolean:
      _bool = right._bool;
      break;
    case Type::kBinary:
    {
      _data.data = right._data.data;
      right._data.data = nullptr;

      _data.len = right._data.len;
      right._data.len = 0;
      break;
    }
    case Type::kObject:
      _array = std::move(right._array);
      break;
  }

  m_type = right.m_type;
  m_status = right.m_status;
  m_key = std::move(right.m_key);

  return *this;
}

KvsType::Type
KvsType::GetType() const noexcept
{
  return m_type;
}

int32_t
KvsType::GetSInt() const noexcept(false)
{
  if(!IsSignedInteger())
  {
    throw std::logic_error("Incompatible type");
  }

  return _integer;
}

uint32_t
KvsType::GetUInt() const noexcept(false)
{
  if(!IsUnsignedInteger())
  {
    throw std::logic_error("Incompatible type");
  }

  return _integer;
}

int64_t
KvsType::GetSInt64() const noexcept(false)
{
  if(m_type != Type::kSInt64)
  {
    throw std::logic_error("Incompatible type");
  }

  return _integer;
}

uint64_t
KvsType::GetUInt64() const noexcept(false)
{
  if(m_type != Type::kUInt64)
  {
    throw std::logic_error("Incompatible type");
  }

  return _integer;
}

float
KvsType::GetFloat() const noexcept(false)
{
  if(m_type != Type::kFloat)
  {
    throw std::logic_error("Incompatible type");
  }

  return _float;
}

double
KvsType::GetDouble() const noexcept(false)
{
  if(m_type != Type::kDouble)
  {
    throw std::logic_error("Incompatible type");
  }

  return _double;
}

std::string
KvsType::GetString() const noexcept(false)
{
  if(m_type != Type::kString)
  {
    throw std::logic_error("Incompatible type");
  }

  return _str;
}

bool
KvsType::GetBool() const noexcept(false)
{
  if(m_type != Type::kBoolean)
  {
    throw std::logic_error("Incompatible type");
  }

  return _bool;
}

void
KvsType::GetBinary(void* data, std::size_t len) const noexcept(false)
{
  if(m_type != Type::kBinary ||
     data == nullptr         ||
     len == 0                ||
     _data.len < len)
  {
    throw std::logic_error("Logic error");
  }

  std::memcpy(data, _data.data, len);
}

KvsType::Status
KvsType::GetStatus() const noexcept
{
  return m_status;
}

std::string
KvsType::GetKey() const noexcept
{
  return m_key;
}

void
KvsType::SetKey(const std::string& name) noexcept
{
  m_key = name;
}

bool
KvsType::IsSignedInteger() const noexcept
{
  if(m_type == Type::kSInt8  ||
     m_type == Type::kSInt16 ||
     m_type == Type::kSInt32 ||
     m_type == Type::kSInt64)
  {
    return true;
  }
  return false;
}

bool
KvsType::IsUnsignedInteger() const noexcept
{
  if(m_type == Type::kUInt8  ||
     m_type == Type::kUInt16 ||
     m_type == Type::kUInt32 ||
     m_type == Type::kUInt64)
  {
    return true;
  }
  return false;
}

void
KvsType::AddArrayItem(const KvsType& kvs) noexcept(false)
{
  auto type = kvs.GetType();

  switch(type)
  {
    case KvsType::Type::kSInt8:
    case KvsType::Type::kSInt16:
    case KvsType::Type::kSInt32:
    {
      _array.emplace_back(kvs.GetSInt());
      break;
    }
    case KvsType::Type::kUInt8:
    case KvsType::Type::kUInt16:
    case KvsType::Type::kUInt32:
    {
      _array.emplace_back(kvs.GetUInt());
      break;
    }
    case KvsType::Type::kSInt64:
    {
      _array.emplace_back(kvs.GetSInt64());
      break;
    }
    case KvsType::Type::kUInt64:
    {
      _array.emplace_back(kvs.GetUInt64());
      break;
    }
    case KvsType::Type::kFloat:
    {
      _array.emplace_back(kvs.GetFloat());
      break;
    }
    case KvsType::Type::kDouble:
    {
      _array.emplace_back(kvs.GetDouble());
      break;
    }
    case KvsType::Type::kString:
    {
      _array.emplace_back(kvs.GetString());
      break;
    }
    case KvsType::Type::kBoolean:
    {
      _array.emplace_back(kvs.GetBool());
      break;
    }
    case KvsType::Type::kBinary:
    {
      _array.emplace_back(kvs.GetBool());
      break;
    }
  }
}

} // namespace api