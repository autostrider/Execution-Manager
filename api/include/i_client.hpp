#ifndef I_CLIENT_HPP
#define I_CLIENT_HPP

#include <any.pb.h>

#include <string>

namespace api
{

class IClient
{
public:
    virtual ~IClient() = default;

    virtual void connect() = 0;
    virtual void sendMessage(const google::protobuf::Any&) = 0;
    virtual std::string receive() = 0;
    virtual bool isConnected() = 0;
    virtual ssize_t getRecvBytes() = 0;
};

}
#endif // I_CLIENT_HPP