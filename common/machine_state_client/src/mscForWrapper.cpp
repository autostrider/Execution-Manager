#include "mscForWrapper.hpp"

namespace machine_state_client
{

MscForWrapper::MscForWrapper(const std::string& path)
    : MachineStateClient(path)
{}

void MscForWrapper::setClient(std::unique_ptr<IClient> client)
{
    std::cout << "MscForWrapper\n";
    MachineStateClient::setClient(std::move(client));
}

}