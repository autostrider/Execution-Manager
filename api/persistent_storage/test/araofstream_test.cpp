#include <araofstream.h>
#include "gtest/gtest.h"
#include <sstream>

using namespace testing;

namespace AraOfStreamTest
{

class AraOfStreamTest :
  public ::testing::Test
{
public:
  void SetUp() override
  {
    ::remove(testFilename.c_str());
    std::ofstream createdFile(testFilename);

    EXPECT_TRUE(createdFile.is_open());

    createdFile << testString;
  }

  void TearDown() override
  {
    ::remove(testFilename.c_str());
  }

  const std::string testFilename{"/tmp/araofstream_test"};
  const std::string testString{"Hello it's test string"};
};

TEST_F(AraOfStreamTest, ShouldCreateFileWithString)
{
  api::araofstream stream(testFilename);

  EXPECT_TRUE(stream.is_open());
}

TEST_F(AraOfStreamTest, ShouldCreateFileWithCString)
{
  api::araofstream stream(testFilename.c_str());

  EXPECT_TRUE(stream.is_open());
}

TEST_F(AraOfStreamTest, ShouldOpenFileWithString)
{
  api::araofstream stream;

  stream.open(testFilename);

  EXPECT_TRUE(stream.is_open());
}

TEST_F(AraOfStreamTest, ShouldOpenFileWithCString)
{
  api::araofstream stream;

  stream.open(testFilename.c_str());

  EXPECT_TRUE(stream.is_open());
}

TEST_F(AraOfStreamTest, ShouldWriteFromFileSuccessfully)
{
  api::araofstream stream(testFilename);

  ASSERT_TRUE(stream.is_open());

  stream << testString;

  EXPECT_EQ(stream.tellp(), testString.length());
}

} // namespace AraIfStreamTest