#include "KvsType.h"
#include <stdexcept>
#include <cstring>

namespace api {

KvsType::KvsType()
: m_type(Type::kNotSet), m_status(Status::kNotFound)
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
  std::memcpy(_data.data, data, len);
  _data.len = len;
}

KvsType::~KvsType()
{
}

KvsType::Type
KvsType::GetType () const noexcept
{
  return m_type;
}

int32_t
KvsType::GetSInt() const noexcept(false)
{
  if(!IsSignedInteger())
  {
    throw std::runtime_error("Incompatible type");
  }

  return _integer;
}

uint32_t
KvsType::GetUInt () const noexcept(false)
{
  if(!IsUnsignedInteger())
  {
    throw std::runtime_error("Incompatible type");
  }

  return _integer;
}

int64_t
KvsType::GetSInt64 () const noexcept(false)
{
  if(m_type != Type::kSInt64)
  {
    throw std::runtime_error("Incompatible type");
  }

  return _integer;
}

uint64_t
KvsType::GetUInt64 () const noexcept(false)
{
  if(m_type != Type::kUInt64)
  {
    throw std::runtime_error("Incompatible type");
  }

  return _integer;
}

float
KvsType::GetFloat () const noexcept(false)
{
  if(m_type != Type::kFloat)
  {
    throw std::runtime_error("Incompatible type");
  }

  return _float;
}

double
KvsType::GetDouble () const noexcept(false)
{
  if(m_type != Type::kDouble)
  {
    throw std::runtime_error("Incompatible type");
  }

  return _double;
}

std::string
KvsType::GetString () const noexcept(false)
{
  if(m_type != Type::kString)
  {
    throw std::runtime_error("Incompatible type");
  }

  return _str;
}

bool
KvsType::GetBool () const noexcept(false)
{
  if(m_type != Type::kBoolean)
  {
    throw std::runtime_error("Incompatible type");
  }

  return _bool;
}

void
KvsType::GetBinary (void* data, std::size_t len) const noexcept(false)
{
  if(m_type != Type::kBoolean)
  {
    throw std::runtime_error("Incompatible type");
  }

  if(data == nullptr ||
     len == 0        ||
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

// template <class Array>
//   void StoreArray (const Array& array) noexcept(false);
//   template <class T>
//   std::vector<T> GetArray () noexcept(false);

//   void AddArrayItem (const KvsType& kvs) noexcept(false);

} // namespace api