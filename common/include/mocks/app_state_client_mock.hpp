#ifndef APPSTATECLIENTMOCK_HPP
#define APPSTATECLIENTMOCK_HPP

#include "i_application_state_client_wrapper.hpp"

#include "gmock/gmock.h"

class AppStateClientMock : public api::IApplicationStateClientWrapper
{
public:
    AppStateClientMock() = default;
    MOCK_METHOD(void, ReportApplicationState, (ApplicationStateManagement::ApplicationState state));
};

#endif // APPSTATECLIENTMOCK_HPP