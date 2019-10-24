#ifndef I_ADAPTIVE_APP_BASE
#define I_ADAPTIVE_APP_BASE

#include <vector>

class IAdaptiveAppBase
{
public:
    virtual ~IAdaptiveAppBase() = default;
    virtual double mean() = 0;
protected:
    virtual std::vector<double> readSensorData() = 0;
};
#endif
