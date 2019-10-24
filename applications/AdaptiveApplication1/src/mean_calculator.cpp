#include "mean_calculator.hpp"

#include <numeric>
#include <random>

double MeanCalculator::mean()
{
    auto data = readSensorData();
    double sum = std::accumulate(data.cbegin(), data.cend(), 0.0);
    return sum / data.size();
}

std::vector<double> MeanCalculator::readSensorData()
{
    const size_t c_numberOfSamples = 50;
    std::vector<double> data(c_numberOfSamples);
    std::random_device rd{};
    std::mt19937 gen{rd()};
    double mu = 10, sigma = 0.5;
    std::normal_distribution<>d(mu, sigma);

    for (size_t k = 0; k < c_numberOfSamples; ++k) {
        data[k] = d(gen);
    }
    return data;
}
