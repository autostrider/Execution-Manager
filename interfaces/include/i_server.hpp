#ifndef I_SERVER_HPP
#define I_SERVER_HPP

#include <memory>

class IClient;
class IServer
{
public:
    virtual ~IServer() = default;

    virtual void start() = 0;
    virtual void stop() = 0;
    virtual bool isStarted() = 0;
    virtual int acceptConnection() = 0;
    virtual void readFromSocket(std::shared_ptr<IClient>) = 0;
};

#endif //I_SERVER_HPP
