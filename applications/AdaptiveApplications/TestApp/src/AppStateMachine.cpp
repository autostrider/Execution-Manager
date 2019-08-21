#include "AppStateMachine.hpp"

#include <algorithm>
#include <iostream>
#include <random>
#include <vector>
#include <cmath>

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
    const size_t numberOfSamples = 2000;
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
    out /= in.size();
    return  out;
}
auto App::getWindows(std::vector<double>& input, const size_t numberOfWindows, const size_t windowSize)
{
    std::vector<std::vector<double>> vec(numberOfWindows);
    vec.shrink_to_fit();

    // each iteration of this loop process next set of n elements
    // and store it in a vector at k'th index in vec
    for (size_t k = 0; k < numberOfWindows; ++k)
    {
        // get range for next set of n elements
        auto start_itr = std::next(input.cbegin(),
                                   static_cast<int>(k*windowSize));
        auto end_itr = std::next(input.cbegin(),
                                 static_cast<int>(k*windowSize + windowSize));

        // allocate memory for the sub-vector
        vec[k].resize(windowSize);

        // code to handle the last sub-vector as it might
        // contain less elements
        if (k*windowSize + windowSize > input.size()) {
            end_itr = input.cend();
            vec[k].resize(input.size() - k*windowSize);
        }

        // copy elements from the input range to the sub-vector
        std::copy(start_itr, end_itr, vec[k].begin());
    }
    return vec;
}

std::vector<double> App::filterData(std::vector<double>& input)
{
    // split vector into sub-vectors each of size n
    size_t numberOfWindows = 8;

    // determine number of sub-vectors of size n
    size_t windowSize = (input.size() - 1) / numberOfWindows + 1;

    // create array of vectors to store the sub-vectors
    auto windows = getWindows(input, numberOfWindows, windowSize);

    input.clear();

    std::vector<double> out(numberOfWindows);

    for (size_t window = 0; window < numberOfWindows; ++window)
    {
        out[window] = mean(windows[window]);
    }
    return out;
}

struct kInitialize : public App
{
    virtual void entry() override;
};

struct kRun : public App
{
    virtual void entry() override;
};

struct kTerminate : public App
{
    virtual void entry() override;
};

void kInitialize::entry()
{
    std::cout << "CustomInitialize::entry()\n";
    if (!_rawData.empty())
    {
        std::cout << "Data is already initialized!\n";
        return;
    }
    std::cout << "initialize data...\n";
    _rawData = readSensorData();
}

void kRun::entry()
{
    std::cout << "CustomRun::entry()\n";
    mean(_rawData);
    //_filteredData = filterData(_rawData);
}


void kTerminate::entry()
{
    std::cout << "CustomTerminate::entry()\n";
    _rawData.clear();
    _filteredData.clear();
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
