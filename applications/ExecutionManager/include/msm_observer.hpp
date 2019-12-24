#ifndef MSM_OBSERVER_HPP
#define MSM_OBSERVER_HPP

#include "i_app_observer.hpp"
#include <list>

namespace ExecutionManager
{

class MsmObserver final : public IAppObserver
{
public:
  void observe(const std::string& app) override;
  void subscribe(Listener object) override;
  void detach(const std::string& app) override;
private:
  void run();
private:
  std::list<Listener> m_subscribers;
  std::string m_msmName;
};

}

#endif // MSM_OBSERVER_HPP
