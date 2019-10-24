#ifndef ADAPTIVE_APP_BASE
#define ADAPTIVE_APP_BASE

#include "i_adaptive_app_base.hpp"

class AdaptiveAppBase : public IAdaptiveAppBase
{
public:
    virtual double mean() override;
private:
    virtual std::vector<double> readSensorData() override;
};
#endif
