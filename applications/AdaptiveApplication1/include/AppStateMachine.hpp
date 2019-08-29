#ifndef __APP_STATE_MACHINE__
#define __APP_STATE_MACHINE__

#include <AppStateEvents.hpp>
#include <iostream>
#include <vector>

struct App : public tinyfsm::Fsm<App>
{
    void react(Initializing const&);
    void react(Running const&);
    void react(Terminating const&);
    virtual void entry(void) = 0;
    void exit(void){}
    virtual ~App(){}
protected:
    static std::vector<double> _rawData;

    std::vector<double> readSensorData();
    double mean(const std::vector<double>& in);
    void printVector(const std::vector<double>& vec) const;
};

struct kInitialize : public App
{
    void entry() override;
};

struct kRun : public App
{
    void entry() override;
};

struct kTerminate : public App
{
    void entry() override;
};

#endif
