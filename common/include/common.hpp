#ifndef COMMON
#define COMMON

#include <string>
#include <map>
#include <queue>
#include <mutex>
#include <memory>
#include <functional>
#include <condition_variable>

std::string 
getServiceName(const std::string& appName, const std::string& procName);

std::string getAppBinaryPath(pid_t pid);

std::string parseServiceName(const std::string& path);


class ThreadQueue
{
public:
    ThreadQueue() = default;
    void addMethod(std::function<void()> func);
    void getExecutor();
    bool isEmpty();
private:
    std::queue<std::function<void()>> m_methodsQueue;
    std::mutex m_queueMutex;
    std::condition_variable m_condition;
};

enum class THREADS
{
  EM_SERVER
};

static std::map<THREADS, std::shared_ptr<ThreadQueue>> threadsMap
{
    {THREADS::EM_SERVER, std::make_shared<ThreadQueue>()}
};
#endif // COMMON
