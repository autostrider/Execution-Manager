#ifndef I_CLIENT_HPP
#define I_CLIENT_HPP

#include <any.pb.h>

#include <string>

class IClient
{
public:
    virtual void connect() = 0;
    virtual void sendMessage(const google::protobuf::Any&) = 0;
    virtual std::string receive() = 0;
    virtual bool isConnected() = 0;
};

#endif // I_CLIENT_HPP