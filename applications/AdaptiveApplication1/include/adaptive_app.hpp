#ifndef ADAPTIVE_APP
#define ADAPTIVE_APP

#include <i_adaptive_app.hpp>
#include <constants.hpp>

#include <memory>
#include <atomic>

using namespace constants;

using ComponentState = std::string;
using ComponentClientReturnType = enums::ComponentClientReturnType;

namespace api
{
    class IStateFactory;
    class IMeanCalculator;
}
namespace application_state
{
    class IApplicationStateClientWrapper;
}

namespace component_client
{
    class IComponentClientWrapper;
}

class AdaptiveApp : public api::IAdaptiveApp
{
public:
    AdaptiveApp(std::unique_ptr<api::IStateFactory> factory,
                std::unique_ptr<application_state::IApplicationStateClientWrapper> appClient,
                std::unique_ptr<component_client::IComponentClientWrapper> compClient,
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
    std::unique_ptr<component_client::IComponentClientWrapper> m_componentClient;
    std::unique_ptr<api::IMeanCalculator> m_meanCalculator;
    bool m_eventModeEnabled;
};
#endif
