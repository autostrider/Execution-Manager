#ifndef ADAPTIVE_APP_BASE_MOCK_HPP
#define ADAPTIVE_APP_BASE_MOCK_HPP

#include <i_mean_calculator.hpp>

#include "gmock/gmock.h"

class MeanCalculatorMock : public api::IMeanCalculator
{
public:
    MOCK_METHOD0(mean, double());
};


#endif
