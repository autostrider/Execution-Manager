#include "kvstype.h"

#include "gtest/gtest.h"
#include <stdexcept>

using namespace testing;
using namespace per;

class KvsTypeTests : public Test
{
protected:
  std::string data = "some_data";
};

TEST_F(KvsTypeTests, ShouldReturnCorrectTypesForObjectsWithAndWithoutData)
{
  KvsType emptyObj = KvsType{};
  KvsType kvs = KvsType{data};

  ASSERT_EQ(KvsType::Type::kString, kvs.GetType());
  ASSERT_EQ(KvsType::Type::kNotSet, emptyObj.GetType());
}

TEST_F(KvsTypeTests, ShouldSuccessfulyGetDataWhenKvsWasCreatedWithData)
{
  KvsType kvs = KvsType{data};

  ASSERT_EQ(data, kvs.GetString());
}

TEST_F(KvsTypeTests, ShouldThrowWhenKvsCreatedWithotData)
{
  KvsType kvs;

  ASSERT_THROW(kvs.GetString(),
          std::runtime_error);
}