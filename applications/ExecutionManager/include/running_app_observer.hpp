#ifndef APP_OBSERVER_HPP
#define APP_OBSERVER_HPP

#include <memory>
#include <set>
#include <list>
#include <atomic>
#include <thread>
#include <functional>

namespace ExecutionManager
{

class IApplicationHandler;

using Listener = std::function<void(const std::string&)>;

class RunningAppObserver
{
public:
  RunningAppObserver(std::unique_ptr<IApplicationHandler> appHandler);
  void observe(const std::string& app);
  void subscribe(Listener object);
  void detach(const std::string& app);

  virtual ~RunningAppObserver();
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
