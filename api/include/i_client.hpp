#ifndef I_CLIENT_HPP
#define I_CLIENT_HPP

#include <google/protobuf/message.h>

#include <string>

namespace api
{

class IClient
{
public:
    virtual ~IClient() = default;

    virtual void connect() = 0;
    virtual ssize_t receive(std::string&) = 0;
    virtual bool isConnected() = 0;
    virtual ssize_t getRecvBytes() = 0;
    virtual void sendMessage(const google::protobuf::Message& context) = 0;
    virtual int getClientFd() = 0;
};

}
#endif // I_CLIENT_HPP