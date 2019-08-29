#include "AppStateMachine.hpp"

#include <algorithm>
#include <iostream>
#include <random>
#include <vector>
#include <cmath>

std::vector<double> App::_rawData{};

void App::printVector(const std::vector<double>& vec)
{
    for (const auto& item : vec) {
        std::cout << item << " | ";
    }
    std::cout << "\n";
}

std::vector<double> App::readSensorData()
{
    std::cout << "Read data from sensors\n";
    const size_t numberOfSamples = 100000000;
    std::vector<double> data(numberOfSamples);

    std::random_device rd{};
    std::mt19937 gen{rd()};
    double mu = 10, sigma = 0.5;
    std::normal_distribution<>d(mu, sigma);

    for (size_t k = 0; k < numberOfSamples; ++k) {
        data[k] = d(gen);
    }
    return data;
}
double App::mean(const std::vector<double>& in)
{
    double out = std::accumulate(in.cbegin(), in.cend(), 0);
    std::cout << "";
    out /= in.size();
    return  out;
}

void kInitialize::entry()
{
    std::cout << "CustomInitialize::entry()\n";
    std::cout << "initialize data...\n";
    App::getData() = readSensorData();
}

void kRun::entry()
{
    std::cout << "CustomRun::entry()\n";
    std::cout << "calc mean of:";
    std::cout << "" << mean(App::getData()) << "\n";
}


void kTerminate::entry()
{
    std::cout << "CustomTerminate::entry()\n";
    std::cout << "Clear data\n";
    App::getData().clear();
}

void App::react(const kInitializing &)
{
    transit<kInitialize>();
}
void App::react(const kRunning &)
{
    transit<kRun>();
}

void App::react(const kTerminating &)
{
    transit<kTerminate>();
}

FSM_INITIAL_STATE(App, kInitialize)
