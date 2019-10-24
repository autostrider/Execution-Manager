#ifndef I_ADAPTIVE_APP_BASE
#define I_ADAPTIVE_APP_BASE

class IMeanCalculator
{
public:
    virtual ~IMeanCalculator() = default;
    virtual double mean() = 0;
};
#endif
