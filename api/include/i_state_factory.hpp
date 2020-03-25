#ifndef I_STATE_FACTORY_HPP
#define I_STATE_FACTORY_HPP

#include "i_state.hpp"

#include <memory>

namespace api
{

class IAdaptiveApp;

class IStateFactory
{
public:
    virtual~ IStateFactory() = default;

    virtual std::unique_ptr<IState> createInit(IAdaptiveApp &app) const = 0;
    virtual std::unique_ptr<IState> createRun(IAdaptiveApp &app) const = 0;
    virtual std::unique_ptr<IState> createShutDown(IAdaptiveApp &app) const = 0;
    virtual std::unique_ptr<IState> createSuspend(IAdaptiveApp &app) const = 0;
};

}
#endif // I_STATE_FACTORY_HPP
