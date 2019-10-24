#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "mean_calculator.hpp"

using namespace testing;

TEST(MeanCalculatorTest, shouldCalculateMean)
{
    const double mu = 10;
    const double sigma = 0.5;

    MeanCalculator app;
    ASSERT_TRUE(::abs(app.mean()-mu) < sigma);
}
