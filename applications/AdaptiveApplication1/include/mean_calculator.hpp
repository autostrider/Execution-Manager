#ifndef MEAN_CALCULATOR
#define MEAN_CALCULATOR

#include "i_mean_calculator.hpp"

#include <vector>

namespace api {

class MeanCalculator : public IMeanCalculator
{
public:
    double mean() override;
private:
    std::vector<double> readSensorData();
};

}

#endif