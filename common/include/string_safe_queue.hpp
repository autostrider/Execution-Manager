#ifndef EXECUTION_MANAGER_STRINGSAFEQUEUE_HPP
#define EXECUTION_MANAGER_STRINGSAFEQUEUE_HPP

#include <queue>
#include <string>
#include <condition_variable>
#include <mutex>

class StringSafeQueue
{
public:
  void push(const std::string& value);
  std::string pop();
private:
  std::queue<std::string> m_data;
  std::mutex m_mut;
  std::condition_variable m_condVar;
};


#endif //EXECUTION_MANAGER_STRINGSAFEQUEUE_HPP