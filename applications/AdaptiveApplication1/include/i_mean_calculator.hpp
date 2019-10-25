#ifndef I_MEAN_CALCULATOR
#define I_MEAN_CALCULATOR

class IMeanCalculator
{
public:
    virtual ~IMeanCalculator() = default;
    virtual double mean() = 0;
};
#endif
