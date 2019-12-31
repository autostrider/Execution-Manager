#include "initializing_app_observer.hpp"
#include "application_handler.hpp"

namespace ExecutionManager
{

InitializingAppObserver::InitializingAppObserver(std::unique_ptr<IApplicationHandler> appHandler)
    : RunningAppObserver(std::move(appHandler))
{
}

void InitializingAppObserver::run(std::unique_ptr<IApplicationHandler> appHandler)
{
    while (m_isRunning)
    {
        constexpr int timesToPollApps = 6;
        constexpr std::chrono::seconds oneSecond{1};
        std::this_thread::sleep_for(oneSecond);
        for (const auto& app : m_appsToObserve)
        {
            bool started = false;
            for (int i = 0; i < timesToPollApps; ++i)
            {
                if (appHandler->isActiveProcess(app))
                {
                    started = true;
                    break;
                }
            }

            if (!started)
            {
                for (const auto& listener: m_listeners)
                {
                    listener(app);
                }
            }
        }
    }
}

}
