#ifndef ADAPTIVE_APP
#define ADAPTIVE_APP

#include <i_adaptive_app.hpp>
#include <i_component_client_wrapper.hpp>
#include <i_component_server_wrapper.hpp>
#include <constants.hpp>

#include <memory>
#include <atomic>

using namespace component_client;
using namespace component_server;
using namespace constants;

namespace api
{
    class IStateFactory;
    class IMeanCalculator;
    class IComponentServer;
}
namespace application_state
{
    class IApplicationStateClientWrapper;
}


class AdaptiveApp : public api::IAdaptiveApp
{
public:
    AdaptiveApp(std::unique_ptr<api::IStateFactory> factory,
                std::unique_ptr<application_state::IApplicationStateClientWrapper> appClient,
                std::unique_ptr<IComponentClientWrapper> compClient,
                std::unique_ptr<IComponentServerWrapper> compServer,
                std::unique_ptr<api::IMeanCalculator> meanCalculator,
                bool eventModeEnabled = false);
    virtual ~AdaptiveApp() override = default;

    void init() override;
    void run() override;
    void terminate() override;
    void performAction() override;

    double mean();
    void reportApplicationState(application_state::ApplicationStateClient::ApplicationState state) override;

private:
    void suspend();
    void transitToNextState(IAdaptiveApp::FactoryFunc nextState) override;
    bool isValid(const ComponentState& state) const;
    bool shouldResume(const ComponentState& state) const;
    bool shouldSuspend(const ComponentState& state) const;
    void stateUpdateHandler(ComponentState const& state);
    void pollComponentState();

    ComponentClientReturnType setComponentState(ComponentState const& state);
    ComponentState m_componentState = COMPONENT_STATE_ON;

    std::unique_ptr<api::IStateFactory> m_factory;
    std::unique_ptr<api::IState> m_currentState;
    std::unique_ptr<application_state::IApplicationStateClientWrapper> m_appClient;
    std::unique_ptr<IComponentClientWrapper> m_componentClient;
    std::unique_ptr<IComponentServerWrapper> m_componentServer;
    std::unique_ptr<api::IMeanCalculator> m_meanCalculator;
    bool m_eventModeEnabled;
};
#endif
