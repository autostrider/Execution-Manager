#ifndef ASC_FOR_WRAPPER_HPP
#define ASC_FOR_WRAPPER_HPP

#include "application_state_client.h"

namespace application_state
{

class AscForWrapper : public ApplicationStateClient
{
public:
    void setClient(std::unique_ptr<IClient> client);
};

}

#endif // MSC_FOR_WRAPPER_HPP