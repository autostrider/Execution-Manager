#include "safe_queue.hpp"

namespace common
{

void SafeQueue::push(const std::string &value)
{
  std::lock_guard<std::mutex> lck{m_mut};
  m_data.push(value);
  m_condVar.notify_one();
}

std::string SafeQueue::pop()
{
  std::string res;
  {
    std::unique_lock<std::mutex> lck{m_mut};
    m_condVar.wait(lck, [this] { return !m_data.empty(); });
    res = m_data.front();
    m_data.pop();
  }
  return res;
}

}