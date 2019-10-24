#ifndef ADAPTIVE_APP_BASE_MOCK_HPP
#define ADAPTIVE_APP_BASE_MOCK_HPP

#include "gmock/gmock.h"

#include "adaptive_app_base.hpp"

class AppBaseMock : public AdaptiveAppBase
{
public:
    MOCK_METHOD(double, mean,());
    MOCK_METHOD(std::vector<double>, readSensorData, ());
};


#endif
