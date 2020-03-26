#ifndef I_MEAN_CALCULATOR
#define I_MEAN_CALCULATOR

namespace api
{

class IMeanCalculator
{
public:
    virtual ~IMeanCalculator() = default;
    virtual double mean() = 0;
};

}

#endif