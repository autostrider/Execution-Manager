#ifndef APP_STATE_CLIENT_MOCK_HPP
#define APP_STATE_CLIENT_MOCK_HPP

#include <i_application_state_client_wrapper.hpp>

#include "gmock/gmock.h"

namespace application_state
{

class AppStateClientMock : public IApplicationStateClientWrapper
{
public:
    AppStateClientMock() = default;
    MOCK_METHOD1(ReportApplicationState, void(ApplicationStateManagement::ApplicationState state));
};

} // namespace application_state

#endif // APP_STATE_CLIENT_MOCK_HPP