#include <arafstream.h>
#include "gtest/gtest.h"
#include <sstream>

using namespace testing;

namespace AraFStreamTest
{

class AraFStreamTest :
  public ::testing::Test
{
public:
  void SetUp() override
  {
    ::remove(testFilename.c_str());
    std::ofstream createdFile(testFilename);
  }

  void TearDown() override
  {
    ::remove(testFilename.c_str());
  }

  const std::string testFilename{"/tmp/arafstream_test"};
  const std::string testString{"Hello it's test string"};
};

TEST_F(AraFStreamTest, ShouldCreateFileWithString)
{
  api::arafstream stream(testFilename);

  EXPECT_TRUE(stream.is_open());
}

TEST_F(AraFStreamTest, ShouldCreateFileWithCString)
{
  api::arafstream stream(testFilename.c_str());

  EXPECT_TRUE(stream.is_open());
}

TEST_F(AraFStreamTest, ShouldOpenFileWithString)
{
  api::arafstream stream;

  stream.open(testFilename);

  EXPECT_TRUE(stream.is_open());
}

TEST_F(AraFStreamTest, ShouldOpenFileWithCString)
{
  api::arafstream stream;

  stream.open(testFilename.c_str());

  EXPECT_TRUE(stream.is_open());
}

TEST_F(AraFStreamTest, ShouldWriteAndReadFromFileSuccessfully)
{
  api::arafstream stream(testFilename);

  ASSERT_TRUE(stream.is_open());

  stream << testString;

  stream.flush();
  stream.seekp(0);

  std::stringstream iss;
  for(std::string line; std::getline(stream, line); iss << line) {}

  EXPECT_EQ(iss.rdbuf()->str(), testString);
}

} // namespace AraFStreamTest