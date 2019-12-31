#ifndef NEW_APP_OBSERVER_HPP
#define NEW_APP_OBSERVER_HPP

#include "running_app_observer.hpp"

namespace ExecutionManager
{

class InitializingAppObserver final : public RunningAppObserver
{
public:
    InitializingAppObserver(std::unique_ptr<IApplicationHandler> appHandler);
private:
    void run(std::unique_ptr<IApplicationHandler> appHandler) override;
};

}
#endif // NEW_APP_OBSERVER_HPP
