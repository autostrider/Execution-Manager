#ifndef MSC_FOR_WRAPPER_HPP
#define MSC_FOR_WRAPPER_HPP

#include "machine_state_client.h"

namespace machine_state_client
{

class MscForWrapper : public MachineStateClient
{
public:
    MscForWrapper(const std::string& path);
    void setClient(std::unique_ptr<IClient> client);
};

}

#endif // MSC_FOR_WRAPPER_HPP