#ifndef I_APP_OBSERVER_HPP
#define I_APP_OBSERVER_HPP

#include <string>
#include <functional>

namespace ExecutionManager
{

using Listener = std::function<void(const std::string&)>;

class IAppObserver
{
public:
  virtual void observe(const std::string& app) = 0;
  virtual void subscribe(Listener object) = 0;
  virtual void detach(const std::string& app) = 0;
  virtual ~IAppObserver() = default;
};

}

#endif // I_APP_OBSERVER_HPP
