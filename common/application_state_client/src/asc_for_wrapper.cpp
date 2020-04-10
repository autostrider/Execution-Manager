#include "asc_for_wrapper.hpp"

namespace application_state
{

void AscForWrapper::setClient(std::unique_ptr<IClient> client)
{
    ApplicationStateClient::setClient(std::move(client));
}

}