#ifndef ADAPTIVE_APP
#define ADAPTIVE_APP

#include <i_adaptive_app.hpp>
#include <i_component_client_wrapper.hpp>

#include <memory>
#include <atomic>

namespace api
{
class IStateFactory;
class IApplicationStateClientWrapper;
}

class IMeanCalculator;

class AdaptiveApp : public api::IAdaptiveApp
{
public:
    AdaptiveApp(std::unique_ptr<api::IStateFactory> factory,
                std::unique_ptr<api::IApplicationStateClientWrapper> appClient,
                std::unique_ptr<api::IComponentClientWrapper> compClient,
                std::unique_ptr<IMeanCalculator> meanCalculator);
    virtual ~AdaptiveApp() override = default;

    void init() override;
    void run() override;
    void terminate() override;

    double mean();
    void reportApplicationState(api::ApplicationStateClient::ApplicationState state) override;

    api::ComponentClientReturnType getComponentState
    (api::ComponentState& state) noexcept;


    void confirmComponentState
    (api::ComponentState state, api::ComponentClientReturnType status) noexcept;

    void setComponentState(const api::ComponentState &componentState);
    api::ComponentState getComponentState() const;

private:
    void transitToNextState(IAdaptiveApp::FactoryFunc nextState) override;

    std::unique_ptr<api::IStateFactory> m_factory;
    std::unique_ptr<api::IState> m_currentState;
    std::unique_ptr<api::IApplicationStateClientWrapper> m_appClient;
    std::unique_ptr<api::IComponentClientWrapper> m_componentClient;
    std::unique_ptr<IMeanCalculator> m_meanCalculator;

    api::ComponentState m_componentState = "lalalala";
};
#endif
