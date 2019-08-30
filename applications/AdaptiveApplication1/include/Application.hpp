#ifndef __APPLICATION__
#define __APPLICATION__

#include <AppStateMachine.hpp>

class Application
{
public:
    static App &getInstance();

private:
    Application() = default;
    ~Application() = default;
    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;
    Application(Application&&) = delete;
    Application& operator=(Application&&) = delete;
};
#endif
