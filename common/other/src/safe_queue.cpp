#include "safe_queue.hpp"

namespace common
{

void SafeQueue::push(const std::string &value)
{
  std::lock_guard<std::mutex> lck{m_mut};
  m_data.push(value);
}

bool SafeQueue::pop(std::string& payload)
{
  bool result {false};

  std::lock_guard<std::mutex> lck{m_mut};
  if (!m_data.empty())
  {
    payload = m_data.front();
    m_data.pop();
    result = true;
  }

  return result;
}

}
