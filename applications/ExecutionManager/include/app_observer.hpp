#ifndef APP_OBSERVER_HPP
#define APP_OBSERVER_HPP

#include <string>
#include <functional>
#include <memory>
#include <set>
#include <list>
#include <atomic>
#include <thread>

class IOsInterface;

namespace ExecutionManager
{

using Listener = std::function<void(const std::string&)>;

class AppObserver
{
public:
  AppObserver(std::unique_ptr<IOsInterface> os);
  void observe(const std::string& app); 
  void subscribe(Listener object); 
  void detach(const std::string& app);

  ~AppObserver();
private:
  void run(std::unique_ptr<IOsInterface> os);
private: 
  std::atomic<bool> m_isRunning;
  std::thread m_worker;
  std::set<std::string> m_appsToObserve;
  std::list<Listener> m_listeners;
};

}

#endif // APP_OBSERVER_HPP
