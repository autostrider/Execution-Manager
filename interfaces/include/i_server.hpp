#ifndef I_SERVER_HPP
#define I_SERVER_HPP

class IServer
{
public:
    virtual ~IServer() = default;

    virtual bool start() = 0;
    virtual void stop() = 0;
};

#endif //I_SERVER_HPP
