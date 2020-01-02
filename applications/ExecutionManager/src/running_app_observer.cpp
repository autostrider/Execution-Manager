#include "running_app_observer.hpp"

#include "application_handler.hpp"
#include <logger.hpp>

namespace ExecutionManager
{

RunningAppObserver::RunningAppObserver(std::unique_ptr<IApplicationHandler> appHandler)
    : m_isRunning{true},
      m_worker{std::thread{&RunningAppObserver::run, this, std::move(appHandler)}}
{ }

void RunningAppObserver::detach(const std::string& app)
{
    std::lock_guard<std::mutex> lk{m_appsMutex};
    m_appsToObserve.erase(app);
}

void RunningAppObserver::observe(const std::string& app)
{
    std::lock_guard<std::mutex> lk{m_appsMutex};
    m_appsToObserve.insert(app);
}

void RunningAppObserver::subscribe(Listener object)
{
    std::lock_guard<std::mutex> lk{m_appsMutex};
    m_listeners.push_back(object);
}

void RunningAppObserver::run(std::unique_ptr<IApplicationHandler> appHandler)
{
    while(m_isRunning)
    {
        {
//            std::unique_lock<std::mutex> lc{m_appsMutex};
            for (const auto& app: m_appsToObserve)
            {
                LOG << "WTF Checking app " << app << " " << (appHandler == nullptr) << " " << m_listeners.size();
                if (!appHandler->isActiveProcess(app))
                {
                    for (auto listener: m_listeners)
                    {
                        LOG << "WTF FAILED " << app;
                        listener(app);
                    }
                }
            }
        }
        std::this_thread::sleep_for(std::chrono::seconds{2});
    }
}

RunningAppObserver::~RunningAppObserver()
{
    m_isRunning = false;
    if (m_worker.joinable())
    {
        m_worker.join();
    }
}

}
