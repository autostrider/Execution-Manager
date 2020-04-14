#ifndef EXECUTION_MANAGER_SAFE_QUEUE_HPP
#define EXECUTION_MANAGER_SAFE_QUEUE_HPP

#include <queue>
#include <string>
#include <condition_variable>
#include <mutex>

namespace common
{

struct ReceivedMessage
{
    std::string data;
    int fd;
};

class SafeQueue
{
public:
  void push(const ReceivedMessage&);
  bool pop(ReceivedMessage& payload);
private:
  std::queue<ReceivedMessage> m_data;
  std::mutex m_mut;
};

}

#endif //EXECUTION_MANAGER_SafeQueue_HPP
