#ifndef APP_STATE_CLIENT_MOCK_HPP
#define APP_STATE_CLIENT_MOCK_HPP

#include "i_application_state_client_wrapper.hpp"

#include "gmock/gmock.h"

class AppStateClientMock : public api::IApplicationStateClientWrapper
{
public:
    AppStateClientMock() = default;
    MOCK_METHOD(void, ReportApplicationState, (ApplicationStateManagement::ApplicationState state));
};

#endif // APP_STATE_CLIENT_MOCK_HPP