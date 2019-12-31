#ifndef APP_OBSERVER_HPP
#define APP_OBSERVER_HPP

#include "i_app_observer.hpp"

#include <memory>
#include <set>
#include <list>
#include <atomic>
#include <thread>


namespace ExecutionManager
{

class IApplicationHandler;

class AppObserver
{
public:
  AppObserver(std::unique_ptr<IApplicationHandler> appHandler);
  void observe(const std::string& app);
  void subscribe(Listener object);
  void detach(const std::string& app);

  ~AppObserver();
protected:
  virtual void run(std::unique_ptr<IApplicationHandler> appHandler);
protected:
  std::atomic<bool> m_isRunning;
  std::thread m_worker;
  std::set<std::string> m_appsToObserve;
  std::list<Listener> m_listeners;
};

}

#endif // APP_OBSERVER_HPP
