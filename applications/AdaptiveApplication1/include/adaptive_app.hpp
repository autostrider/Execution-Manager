#ifndef ADAPTIVE_APP
#define ADAPTIVE_APP

#include <i_adaptive_app.hpp>
#include <i_state_factory.hpp>

#include <vector>


class AdaptiveApp : public api::IAdaptiveApp
{
public:
    AdaptiveApp(std::unique_ptr<api::IStateFactory> factory,
                std::unique_ptr<api::IApplicationStateClientWrapper> client);
    virtual ~AdaptiveApp() = default;

    void run() override;
    void terminate() override;

    double mean();
    void readSensorData();
    void printSensorData() const;
    void reportApplicationState(api::ApplicationStateClient::ApplicationState state) override;

private:
    void transitToNextState(IAdaptiveApp::FactoryFunc nextState);

    const size_t c_numberOfSamples = 50;
    std::vector<double> m_sensorData;
    std::unique_ptr<api::IStateFactory> m_factory;
    std::unique_ptr<api::IState> m_currentState;
    std::unique_ptr<api::IApplicationStateClientWrapper> m_appClient;
};
#endif
