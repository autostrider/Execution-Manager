#ifndef ISTATE_HPP
#define ISTATE_HPP

namespace api
{

class IState
{
public:
    virtual ~IState() = default;

    virtual void enter() = 0;
    virtual void leave() const = 0;
    virtual void performAction() = 0;
};

}
#endif // ISTATE_HPP
