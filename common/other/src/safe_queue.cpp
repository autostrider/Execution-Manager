#include "safe_queue.hpp"

namespace common
{

void SafeQueue::push(const ReceivedMessage& message)
{
  std::lock_guard<std::mutex> lck{m_mut};
  m_data.push(message);
}

bool SafeQueue::pop(ReceivedMessage& payload)
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
