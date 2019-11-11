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
    virtual ~IAdaptiveApp() = default;

    virtual void init() = 0;
    virtual void run() = 0;
    virtual void terminate() = 0;
    virtual void performAction() = 0;
    virtual void reportApplicationState(api::ApplicationStateClient::ApplicationState state) = 0;

protected:
    using FactoryFunc = std::function<std::unique_ptr<IState>(IAdaptiveApp&)>;
    virtual void transitToNextState(IAdaptiveApp::FactoryFunc nextState) = 0;
};

} // namespace api

#endif // IADAPTIVE_APP_HPP
