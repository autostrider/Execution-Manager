#include <Application.hpp>

App &Application::getInstance()
{
    static App instance;
    return  instance;
}
