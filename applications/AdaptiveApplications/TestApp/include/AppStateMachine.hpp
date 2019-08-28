#ifndef __APP_STATE_MACHINE__
#define __APP_STATE_MACHINE__

#include <AppStateEvents.hpp>

#include <iostream>
#include <vector>

struct App : public tinyfsm::Fsm<App>
{
    virtual void react(kInitializing const&);
    virtual void react(kRunning const&);
    virtual void react(kTerminating const&);
    virtual void entry(void) = 0;
    void exit(void){}
    virtual ~App(){}
    static std::vector<double>& getData() {return  App::_rawData;}
protected:
    static std::vector<double> _rawData;

    std::vector<double> readSensorData();
    double mean(const std::vector<double>& in);
    void printVector(const std::vector<double>& vec);
};

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

#endif
