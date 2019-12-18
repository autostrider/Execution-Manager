#ifndef EXECUTION_MANAGER_KVSTYPE_H
#define EXECUTION_MANAGER_KVSTYPE_H

#include <string>

namespace per
{

class KvsType
{
public:
  enum class Type : uint8_t
  {
    KNotSupported = 0,
    kString,
    kNotSet
  };

  enum class Status : uint8_t
  {
    kSuccess = 0,
    kSuccessDefaultValue,
    kNotFound,
    kCheckSumError,
    kGeneralError
  };

public:
  KvsType(const KvsType&) = delete;

  KvsType& operator=(const KvsType&) = delete;
  KvsType(KvsType&&) = default;
  KvsType& operator=(KvsType&&) = default;
  KvsType(std::string value);
  KvsType();

  ~KvsType() = default;
  Type GetType() const noexcept;
  std::string GetString() const noexcept(false);

private:
  Status m_status;
  Type m_type;
  std::string m_value;
};

}

#endif //EXECUTION_MANAGER_KVSTYPE_H
