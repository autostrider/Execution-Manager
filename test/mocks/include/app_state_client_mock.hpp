#ifndef APP_STATE_CLIENT_MOCK_HPP
#define APP_STATE_CLIENT_MOCK_HPP

#include <i_application_state_client_wrapper.hpp>

#include "gmock/gmock.h"

namespace api
{
class AppStateClientMock : public IApplicationStateClientWrapper
{
public:
    MOCK_METHOD(void, ReportApplicationState,(ApplicationStateManagement::ApplicationState), (override));
};

} // namespace api

#endif // APP_STATE_CLIENT_MOCK_HPP