#ifndef __APP_STATE_MACHINE__
#define __APP_STATE_MACHINE__

#include <tinyfsm.hpp>

#include <iostream>
#include <vector>

struct kInitializing : tinyfsm::Event{};

struct kRunning : tinyfsm::Event{};

struct kTerminating : tinyfsm::Event{};


struct App : public tinyfsm::Fsm<App>
{
    virtual void react(kInitializing const&);
    virtual void react(kRunning const&);
    virtual void react(kTerminating const&);
    virtual void entry(void) = 0;
    void exit(void){}
    virtual ~App(){}
protected:
    std::vector<double> _rawData;
    std::vector<double> _filteredData;
    std::vector<double> readSensorData();
    double mean(const std::vector<double>& in);
    auto getWindows(std::vector<double>& input, const size_t numberOfWindows, const size_t windowSize);
    std::vector<double> filterData(std::vector<double>& input);
    void printVector(const std::vector<double>& vec);
};

#endif
