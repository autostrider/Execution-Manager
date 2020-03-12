#include "client_maker.hpp"
#include <constants.hpp>

std::shared_ptr<Client> ClientMaker::makeClientPtr(const int& fd) const
{
    auto clientPtr = std::make_shared<Client>(SOCKET_SERVER_PATH, std::make_unique<ClientSocket>());
    
    clientPtr->setClientFd(fd);
    clientPtr->setConnected(true);
    return clientPtr;
}