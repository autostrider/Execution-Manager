#include <common.hpp>

#include <mocks/os_interface_mock.hpp>

#include "gtest/gtest.h"

using namespace ::testing;

TEST(ThreadSafeQueueTest, checkFunctionsExecution)
{
  StrictMock<OSInterfaceMock> mock;
  {
    InSequence sq;
    EXPECT_CALL(mock, fork());
    EXPECT_CALL(mock, popen(_, _));
  }
  ThreadQueue queue;

  queue.addMethod([&] { mock.fork(); });
  queue.addMethod([&] { mock.popen("File", "open"); });

  while(!queue.isEmpty())
  {
    queue.getExecutor();
  }
}