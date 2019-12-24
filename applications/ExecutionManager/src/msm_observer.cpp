#include "msm_observer.hpp"
#include "application_handler.hpp"
#include "os_interface.hpp"

namespace ExecutionManager
{

MsmObserver::MsmObserver()
  : IAppObserver(),
    m_subscribers{},
    m_msmName{},
    m_isRunning{true}
{
 m_worker = std::thread(&MsmObserver::run, this); 
}


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

void MsmObserver::run()
{
  ApplicationHandler appHandler{std::make_unique<OsInterface>()};
  while (m_isRunning)
  {
    if (!m_msmName.empty() && !appHandler.isActiveProcess(m_msmName))
    {
      for (const auto& subscriber: m_subscribers)
      {
        subscriber(m_msmName);
      }
    }
  }
}

MsmObserver::~MsmObserver()
{
  m_isRunning = false;
  if (m_worker.joinable())
  {
    m_worker.join();
  }
}

}
