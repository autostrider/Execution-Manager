#ifndef I_SERVER_HPP
#define I_SERVER_HPP

#include "i_connection.hpp"

#include <memory>

namespace common
{
    struct ReceivedMessage;
}

namespace api
{

class IServer
{
public:
    virtual ~IServer() = default;

    virtual void start() = 0;
    virtual void stop() = 0;
    virtual bool isStarted() = 0;
    virtual void readFromSocket(std::vector<std::shared_ptr<IConnection>>::iterator&) = 0;
    virtual bool getQueueElement(common::ReceivedMessage& data) = 0;
    virtual void send(const google::protobuf::Message& context, int fd) = 0;
};

}
#endif //I_SERVER_HPP
