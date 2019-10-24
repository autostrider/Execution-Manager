#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "adaptive_app_base.hpp"

using namespace testing;

class AppBaseTest : public ::testing::Test
{
protected:
    AdaptiveAppBase app;
};

TEST_F(AppBaseTest, shouldCalculateMean)
{
    const double mu = 10;
    const double sigma = 0.5;

    ASSERT_TRUE(::abs(app.mean()-mu) < sigma);
}
