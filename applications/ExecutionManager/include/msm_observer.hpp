#ifndef MSM_OBSERVER_HPP
#define MSM_OBSERVER_HPP

#include "i_app_observer.hpp"
#include <list>
#include <thread>
#include <atomic>

namespace ExecutionManager
{

class MsmObserver final : public IAppObserver
{
public:
  MsmObserver();
//  MsmObserver(MsmObserver&&);
  void observe(const std::string& app) override;
  void subscribe(Listener object) override;
  void detach(const std::string& app) override;
  ~MsmObserver() override;
private:
  void run();
private:
  std::list<Listener> m_subscribers;
  std::string m_msmName;
  std::atomic<bool> m_isRunning;
  std::thread m_worker;
};

}

#endif // MSM_OBSERVER_HPP
