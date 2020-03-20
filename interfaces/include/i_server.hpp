#ifndef I_SERVER_HPP
#define I_SERVER_HPP

#include "i_connection.hpp"

#include <memory>

namespace api::socket_handler
{

class IServer
{
public:
    virtual ~IServer() = default;

    virtual void start() = 0;
    virtual void stop() = 0;
    virtual bool isStarted() = 0;
    virtual void readFromSocket(std::shared_ptr<IConnection>) = 0;
};

}
#endif //I_SERVER_HPP
