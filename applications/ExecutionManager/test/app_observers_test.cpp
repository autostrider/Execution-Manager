#include "initializing_app_observer.hpp"
#include "running_app_observer.hpp"
#include "mocks/application_handler_mock.hpp"
#include "mocks/manifest_reader_mock.hpp"

#include "gtest/gtest.h"

using namespace testing;
using ExecutionManager::RunningAppObserver;
using ExecutionManager::ApplicationHandlerMock;
using ExecutionManager::ManifestReaderMock;
using ExecutionManager::InitializingAppObserver;

class AppObserverTest : public Test
{
protected:
    const std::string appName = "appName";
    std::unique_ptr<NiceMock<ApplicationHandlerMock>> appHandlerMock =
            std::make_unique<NiceMock<ApplicationHandlerMock>>();

    std::unique_ptr<NiceMock<ManifestReaderMock>> manifestReaderMock =
            std::make_unique<NiceMock<ManifestReaderMock>>();
};

TEST_F(AppObserverTest, ShouldSuccesfullyRegisterListenerAndApp)
{
    RunningAppObserver obs{std::move(appHandlerMock)};

    obs.subscribe([](const std::string&) {  });
    obs.observe(appName);
    obs.detach(appName);
}

TEST_F(AppObserverTest, ShouldSuccessfulyReactToFailedApp)
{

    {
        InSequence sq;
        ON_CALL(*appHandlerMock, isActiveProcess(appName))
                .WillByDefault(Return(false));
        ON_CALL(*manifestReaderMock, getMachineStates())
                .WillByDefault(Return(std::vector<std::string>{}));
    }

    RunningAppObserver obs{std::move(appHandlerMock)};
    obs.subscribe([&](const std::string& ) { manifestReaderMock->getMachineStates(); });
    obs.observe(appName);
    std::this_thread::sleep_for(std::chrono::seconds{1});
    obs.detach(appName);
}

TEST_F(AppObserverTest, ShouldNotCallListenersWhenAppStarted)
{
    {
        InSequence sq;
        ON_CALL(*appHandlerMock, isActiveProcess(appName))
                .WillByDefault(Return(true));
        ON_CALL(*manifestReaderMock, getMachineStates())
                .WillByDefault(Return(std::vector<std::string>{}));
    }
    InitializingAppObserver obs{std::move(appHandlerMock)};
    obs.subscribe([&](const std::string& ) { manifestReaderMock->getMachineStates(); });
    obs.observe(appName);
    std::this_thread::sleep_for(std::chrono::seconds{1});
}

TEST_F(AppObserverTest, ShouldCallListenersWhenAppStarted)
{
    {
        InSequence sq;
        ON_CALL(*appHandlerMock, isActiveProcess(appName))
                .WillByDefault(Return(false));
        ON_CALL(*manifestReaderMock, getMachineStates())
                .WillByDefault(Return(std::vector<std::string>{}));
    }
    InitializingAppObserver obs{std::move(appHandlerMock)};
    obs.subscribe([&](const std::string& ) { manifestReaderMock->getMachineStates(); });
    obs.observe(appName);
    std::this_thread::sleep_for(std::chrono::seconds{1});
}
