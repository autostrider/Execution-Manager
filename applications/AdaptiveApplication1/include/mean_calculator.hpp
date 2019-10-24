#ifndef ADAPTIVE_APP_BASE
#define ADAPTIVE_APP_BASE

#include "i_mean_calculator.hpp"

#include <vector>

class MeanCalculator : public IMeanCalculator
{
public:
    double mean() override;
private:
    std::vector<double> readSensorData();
};
#endif
