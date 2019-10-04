#ifndef KVS_TYPE_H
#define KVS_TYPE_H

// #include <keyvaluestorage.h>
#include <stdint.h>
#include <string>
#include <iostream>
#include <vector>
#include <algorithm>
#include <functional>

namespace api {

class KvsType
{
public:
  enum class Type : uint8_t
  {
    kNotSupported = 0,
    kFloat,
    kDouble,
    kSInt8,
    kSInt16,
    kSInt32,
    kSInt64,
    kUInt8,
    kUInt16,
    kUInt32,
    kUInt64,
    kString,
    kBinary,
    kBoolean,
    kObject,
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
  KvsType ();
  KvsType (bool value);
  KvsType (int8_t value);
  KvsType (int16_t value);
  KvsType (int32_t value);
  KvsType (int64_t value);
  KvsType (uint8_t value);
  KvsType (uint16_t value);
  KvsType (uint32_t value);
  KvsType (uint64_t value);
  KvsType (float value);
  KvsType (double value);
  KvsType (std::string value);
  KvsType (void* data, std::size_t len);

  ~KvsType ();

  KvsType (const KvsType&) = delete;
  KvsType& operator= (const KvsType&) = delete;

  KvsType (KvsType&& other);
  KvsType& operator= (KvsType&& right);

  Type GetType () const noexcept;

  int32_t GetSInt () const noexcept(false);
  uint32_t GetUInt () const noexcept(false);
  int64_t GetSInt64 () const noexcept(false);
  uint64_t GetUInt64 () const noexcept(false);
  float GetFloat () const noexcept(false);
  double GetDouble () const noexcept(false);
  std::string GetString () const noexcept(false);
  bool GetBool () const noexcept(false);
  void GetBinary (void* data, std::size_t len) const noexcept(false);

  Status GetStatus () const noexcept;

  std::string GetKey () const noexcept;
  void SetKey (const std::string& name) noexcept;

  bool IsSignedInteger () const noexcept;
  bool IsUnsignedInteger () const noexcept;

  template <template <class ...> class Array>
  auto StoreArray(const Array<KvsType>& array) noexcept(false) ->
    decltype(std::begin(std::declval<Array<KvsType>>()),
             std::end(std::declval<Array<KvsType>>()),
             void())
  {
    std::for_each(std::begin(array), std::end(array),
      std::bind(&KvsType::AddArrayItem, this, std::placeholders::_1));
  }

  template <class Array>
  auto StoreArray (const Array& array) noexcept(false) ->
    decltype(std::begin(std::declval<Array>()),
             std::end(std::declval<Array>()),
             void())
  {
    std::transform(
      std::begin(array),
      std::end(array),
      std::back_inserter(_array),
    [](const auto& value)
    {
      return KvsType(value);
    });
  }

  // template <class T>
  // std::vector<T> GetArray () noexcept(false)
  // {
  //   std::vector<KvsType> other;

  //   std::transform(
  //     std::begin(_array),
  //     std::end(_array),
  //     std::back_inserter(other),
  //   [](const auto& value)
  //   {
  //     return KvsType(value);
  //   });

  //   return std::move(other);
  // }

  void AddArrayItem(const KvsType& kvs) noexcept(false);

private:
  Type m_type;
  Status m_status;

  std::string m_key;

  union
  {
    int64_t _integer;
    float _float;
    double _double;
    std::string _str;
    bool _bool;
    struct
    {
      void* data;
      size_t len;
    } _data;
    std::vector<KvsType> _array;
  };
};

} // namespace api
#endif // KVS_TYPE_H
