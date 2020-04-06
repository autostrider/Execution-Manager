#ifndef EXECUTION_MANAGER_SAFE_QUEUE_HPP
#define EXECUTION_MANAGER_SAFE_QUEUE_HPP

#include <queue>
#include <string>
#include <condition_variable>
#include <mutex>

namespace common
{

class SafeQueue
{
public:
  void push(const std::string& value);
  bool pop(std::string& payload);
private:
  std::queue<std::string> m_data;
  std::mutex m_mut;
};

}

#endif //EXECUTION_MANAGER_SafeQueue_HPP
