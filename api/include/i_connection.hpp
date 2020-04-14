#ifndef I_CONNECTION_HPP
#define I_CONNECTION_HPP

#include <google/protobuf/message.h>
#include <string>

namespace api
{

class IConnection
{
public:
    virtual ~IConnection() = default;

    virtual void creatAcceptedClient() = 0;
    virtual int receiveData(std::string&) = 0;
    virtual int getRecvBytes() = 0;
    virtual void sendData(const google::protobuf::Message& context) = 0;
    virtual int getFd() = 0;
};

}

#endif // I_CONNECTION_HPP