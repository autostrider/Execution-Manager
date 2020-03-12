#ifndef I_CLIENT_MAKER_HPP
#define I_CLIENT_MAKER_HPP

#include <client.hpp>

#include <memory>

class IClientMaker
{
public:
    virtual ~IClientMaker() = default;

    virtual std::shared_ptr<Client> makeClientPtr(const int&) const = 0;
};

#endif //I_CLIENT_MAKER_HPP