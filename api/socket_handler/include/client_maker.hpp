#ifndef CLIENT_MAKER_HPP
#define CLIENT_MAKER_HPP

#include <i_client_maker.hpp>

class ClientMaker : public IClientMaker
{
public:
    std::shared_ptr<Client> makeClientPtr(const int&) const override;
};

#endif // CLIENT_MAKER_HPP