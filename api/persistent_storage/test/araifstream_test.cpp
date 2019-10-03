#include <araifstream.h>
#include "gtest/gtest.h"
#include <sstream>

using namespace testing;

namespace AraIfStreamTest
{

class AraIfStreamTest :
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

  const std::string testFilename{"/tmp/araifstream_test"};
  const std::string testString{"Hello it's test string"};
};

TEST_F(AraIfStreamTest, ShouldCreateFileWithString)
{
  api::araifstream stream(testFilename);

  EXPECT_TRUE(stream.is_open());
}

TEST_F(AraIfStreamTest, ShouldCreateFileWithCString)
{
  api::araifstream stream(testFilename.c_str());

  EXPECT_TRUE(stream.is_open());
}

TEST_F(AraIfStreamTest, ShouldOpenFileWithString)
{
  api::araifstream stream;

  stream.open(testFilename);

  EXPECT_TRUE(stream.is_open());
}

TEST_F(AraIfStreamTest, ShouldOpenFileWithCString)
{
  api::araifstream stream;

  stream.open(testFilename.c_str());

  EXPECT_TRUE(stream.is_open());
}

TEST_F(AraIfStreamTest, ShouldReadFromFileSuccessfully)
{
  api::araifstream stream(testFilename);

  ASSERT_TRUE(stream.is_open());

  std::stringstream iss;
  for(std::string line; std::getline(stream, line); iss << line) {}

  EXPECT_EQ(iss.rdbuf()->str(), testString);
}

} // namespace AraIfStreamTest