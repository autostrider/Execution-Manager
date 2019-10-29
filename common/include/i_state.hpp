#ifndef ISTATE_HPP
#define ISTATE_HPP

#include<application_state_client.h>

namespace api
{

class IState
{
public:
    virtual ~IState() = default;
    virtual void enter() = 0;
    virtual void leave() const = 0;
    virtual void performAction(){}
};

}
#endif // ISTATE_HPP
