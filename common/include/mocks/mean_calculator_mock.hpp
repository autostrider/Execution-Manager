#ifndef ADAPTIVE_APP_BASE_MOCK_HPP
#define ADAPTIVE_APP_BASE_MOCK_HPP

#include "gmock/gmock.h"

#include "i_mean_calculator.hpp"

class MeanCalculatorMock : public IMeanCalculator
{
public:
    MOCK_METHOD(double, mean,());
};


#endif
