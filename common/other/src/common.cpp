#include "common.hpp"

#include <fstream>
#include "logger.hpp"
#include <memory>

namespace common
{

std::string
getServiceName(const std::string& appName, const std::string& procName)
{
    return appName + "_" + procName;
}

std::string getAppBinaryPath(pid_t appPid)
{
    static constexpr auto procPath = "/proc/";
    static constexpr auto cmdName = "/cmdline";

    std::ifstream data {procPath + std::to_string(appPid) + cmdName};
    std::string fullCmd;
    data >> fullCmd;

    return fullCmd;
}

std::string parseServiceName(const std::string& path)
{
    static constexpr auto delimiter = '/';

    static const auto processesSize = std::string("/processes/").size();
    auto currPos = path.find_last_of(delimiter);
    std::string processName = path.substr(currPos + 1);

    auto prevPos = currPos - processesSize;
    currPos = path.find_last_of(delimiter, prevPos);
    std::string appName = path.substr(currPos + 1, prevPos - currPos);

    return getServiceName(appName, processName);
}

void ThreadQueue::addMethod(std::function<void ()> func)
{
    {
        std::lock_guard<std::mutex> lk(m_queueMutex);
        m_methodsQueue.push(func);
    }
    m_condition.notify_one();
}

void ThreadQueue::getExecutor()
{
    std::function<void()> func;
    {
        std::unique_lock<std::mutex> lk(m_queueMutex);
        m_condition.wait(lk, [this]{return !m_methodsQueue.empty();});

        func = m_methodsQueue.front();
        m_methodsQueue.pop();
    }
    if (func)
    {
        func();
    }

}

bool ThreadQueue::isEmpty()
{
    std::lock_guard<std::mutex> lk(m_queueMutex);
    bool res = m_methodsQueue.empty();
    m_condition.notify_one();
    return res;

}

}