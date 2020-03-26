#ifndef I_CONNECTION_HPP
#define I_CONNECTION_HPP

#include <string>

namespace api
{

class IConnection
{
public:
    virtual ~IConnection() = default;

    virtual void creatAcceptedClient() = 0;
    virtual std::string receiveData() = 0;
    virtual int getRecvBytes() = 0;
};

}

#endif // I_CONNECTION_HPP