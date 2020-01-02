#include "initializing_app_observer.hpp"
#include "application_handler.hpp"
#include <logger.hpp>

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
            bool started = m_appsToObserve.empty();
            for (int i = 0; i < timesToPollApps; ++i)
            {
                if (appHandler->isActiveProcess(app))
                {
                    LOG << "WTF APP " << app << " ACTIVE";
                    started = true;
                    break;
                }
            }

            if (!started)
            {
                LOG << "WTF APP NOT STARTED" << app;
//                std::lock_guard<std::mutex> lk{m_appsMutex};
                for (const auto& listener: m_listeners)
                {
                    listener(app);
                }
            }
        }
    }
}

}
