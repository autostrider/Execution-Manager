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

    return fullCmd.c_str();
}

std::string parseServiceName(const std::string& path)
{    
    static const std::string delimiter{'/'};
    static const std::string procStr {"processes"};

    static const auto processesSize = procStr.size();
    auto procNamePos = path.find_last_of(delimiter);
    std::string processName{path.substr(procNamePos + 1)};

    auto processesPos = path.find(procStr);

    std::string res;
    if (processesPos != std::string::npos)
    {
        auto appNameEndPos = processesPos - delimiter.size();
        auto appNameStartPos = path.find_last_of(delimiter, (appNameEndPos - delimiter.size())) + delimiter.size();
        auto appNameSize = appNameEndPos - appNameStartPos;

        std::string appName{path.substr(appNameStartPos, appNameSize)};
    
        res = getServiceName(appName, processName);
    }
    else
    {
        res = processName;
    }
    
    return res;
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