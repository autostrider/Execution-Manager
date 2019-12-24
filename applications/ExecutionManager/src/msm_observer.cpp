#include "msm_observer.hpp"

namespace ExecutionManager
{

void MsmObserver::observe(const std::string& app)
{
  m_msmName = app;
}

void MsmObserver::detach(const std::string& app)
{
  m_msmName.clear();
}

void MsmObserver::subscribe(Listener object)
{
  m_subscribers.push_back(object);
}

}
