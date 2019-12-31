#include "app_observer.hpp"

#include "application_handler.hpp"

namespace ExecutionManager
{

AppObserver::AppObserver(std::unique_ptr<IApplicationHandler> appHandler)
  : m_isRunning{true},
    m_worker{std::thread{&AppObserver::run, this, std::move(appHandler)}}
{ }

void AppObserver::detach(const std::string& app)
{
  m_appsToObserve.insert(app);
}

void AppObserver::observe(const std::string& app)
{
  m_appsToObserve.insert(app);
}

void AppObserver::subscribe(Listener object)
{
  m_listeners.push_back(object);
}

void AppObserver::run(std::unique_ptr<IApplicationHandler> appHandler)
{
  while(m_isRunning)
  {
    for (const auto& app: m_appsToObserve)
    {
      if (!appHandler->isActiveProcess(app))
      {
        for (auto& listener: m_listeners)
        {
          listener(app);
        }
      }
    }
    std::this_thread::sleep_for(std::chrono::seconds{1});
  }
}

AppObserver::~AppObserver()
{
  m_isRunning = false;
  if (m_worker.joinable())
  {
    m_worker.join();
  }
}

}
