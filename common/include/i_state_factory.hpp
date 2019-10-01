#ifndef ISTATEFACTORY_HPP
#define ISTATEFACTORY_HPP

#include "i_state.hpp"
#include "i_adaptive_app.hpp"

#include <memory>

namespace api
{

class IStateFactory
{
public:
    virtual std::unique_ptr<IState> createInit(IAdaptiveApp &app) const = 0;
    virtual std::unique_ptr<IState> createRun(IAdaptiveApp &app) const = 0;
    virtual std::unique_ptr<IState> createShutDown(IAdaptiveApp &app) const = 0;

    virtual~ IStateFactory() = default;
};

}
#endif // ISTATEFACTORY_HPP