#include <mean_calculator.hpp>

#include "gtest/gtest.h"
#include "gmock/gmock.h"

using namespace testing;

TEST(MeanCalculatorTest, shouldCalculateMean)
{
    const double mu = 10;
    const double sigma = 0.5;

    api::MeanCalculator meanCalculator;
    ASSERT_TRUE(::abs(meanCalculator.mean()-mu) < sigma);
}
