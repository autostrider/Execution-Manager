#ifndef IADAPTIVE_APP_HPP
#define IADAPTIVE_APP_HPP

#include <functional>
#include <vector>
#include <memory>

#include <application_state_client.h>

namespace api {

class IState;
class IStateFactory;

class IAdaptiveApp
{
public:
    IAdaptiveApp(std::unique_ptr<IStateFactory> factory,
                std::unique_ptr<api::IApplicationStateClientWrapper> client);
    virtual ~IAdaptiveApp() = default;

    virtual void run() = 0;
    virtual void terminate() = 0;

    void reportApplicationState(api::ApplicationStateClient::ApplicationState state);

protected:
    using FactoryFunc = std::function<std::unique_ptr<IState>(IAdaptiveApp&)>;

    void transitToNextState(IAdaptiveApp::FactoryFunc nextState);

    std::unique_ptr<IStateFactory> m_factory;
    std::unique_ptr<IState> m_currentState;
    std::unique_ptr<api::IApplicationStateClientWrapper> m_appClient;    
};

} // namespace api

#endif // IADAPTIVE_APP_HPP