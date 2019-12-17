#include "kvstype.h"

#include <stdexcept>

std::string per::KvsType::GetString() const noexcept(false)
{
  if (Status::kSuccess == m_status)
  {
    return m_value;
  }
  else
  {
    throw std::runtime_error{"No value in Kvs pair"};
  }
}

per::KvsType::KvsType(std::string value)
  : m_status{Status::kSuccess},
    m_type{Type::kString},
    m_value{std::move(value)}
{

}

void per::KvsType::SetKey(const std::string &name) noexcept
{
  m_key = name;
}

per::KvsType::KvsType()
  : m_status{Status::kNotFound}
{

}
