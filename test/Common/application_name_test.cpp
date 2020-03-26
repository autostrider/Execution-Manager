#include <common.hpp>

#include "gtest/gtest.h"

using namespace common;
using namespace testing;

using testParamType = std::pair<std::string, std::string>;
class ParseServiceNameTest: public TestWithParam<std::pair<std::string, std::string>>
{
};

std::vector<std::pair<std::string, std::string>> testData = 
{
  {"/path/to/Application/processes/Process", "Application_Process"},
  {"process", "process_process"},
  {"/some/app/path/process/", "pa_"},
  {"app/process", "process_process"}

};

INSTANTIATE_TEST_SUITE_P(ParseServiceName, 
                         ParseServiceNameTest, 
                         ValuesIn(testData));

TEST_P(ParseServiceNameTest, ShouldReturnServiceNameFromString)
{
  auto testParam = GetParam();

  ASSERT_EQ(parseServiceName(testParam.first), testParam.second);
}