#include "execution_manager.hpp"
#include "execution_manager_server.hpp"
#include "execution_manager_client.hpp"
#include "msm_handler.hpp"
#include "os_interface.hpp"
#include "application_handler.hpp"
#include "msm_handler.hpp"
#include <manifest_reader.hpp>

#include <kj/async-io.h>
#include <async-io-unix.c++>
#include <async-unix.h>
#include <machine_state_management.capnp.h>
#include <capnp/rpc-twoparty.h>
#include <stdio.h>
#include <async.h>

extern "C" {
    OsInterface *os_new() { return new OsInterface(); }
    void os_delete(OsInterface *os) { delete os; }
}

extern "C" {
    char* path = "./bin/applications/";
    char* serviceName = "msm";
    ApplicationHandler *ah_new(OsInterface *os, char *path) { return new ApplicationHandler(os, path); }

    void ah_delete(ApplicationHandler *ah) { delete ah; }

    bool ah_startProcess(ApplicationHandler *ah, const char *serviceName) { ah->startProcess(serviceName); }
}

extern "C" {
    AsyncIoContext *io_context_new() { return new AsyncIoContext(); }
    void io_context_delete(AsyncIoContext *io_context) { delete io_context; }

    AsyncIoContext io_setupAsyncIo(AsyncIoContext *io_context) {
    <heap> LowLevelAsyncIoProviderImpl
    *lowLevel = ll_io_implementation_new();

    heap <AsyncIoProviderImpl> provider = io_provider_implementation_new(*lowLevel);

    EventPort *eventPort = eventPort_new();
    EventLoop *eventLoop = eventLoop_new(eventPort);
    WaitScope *waitScope = waitScope_new(eventLoop);
    UnixEventPort *ueventPort = ueventPort_new();
    WaitScope wait_Scope = lowLevel->ll_getWaitScope(waitScope);
    UnixEventPort &u_eventPort = lowLevel->ll_getEventPort(ueventPort);

    return {kj::mv(lowLevel), kj::mv(ioProvider), wait_Scope, u_eventPort};

    return io_context;
    }
}

extern "C" {
    LowLevelAsyncIoProviderImpl *ll_io_implementation_new() { return new LowLevelAsyncIoProviderImpl(); }
    void ll_io_implementation_delete(LowLevelAsyncIoProviderImpl *ll_io_implementation) { delete ll_io_implementation; }

    WaitScope &ll_getWaitScope(WaitScope *waitScope) { return waitScope; }
    UnixEventPort ll_getEventPort(EventPort *ep) { return ep; }
}

extern "C" {
    EventPort *eventPort_new() { return new EventPort(); }
    void eventPort_delete(EventPort *eventPort) { delete eventPort; }
}

extern "C" {
    UnixEventPort *ueventPort_new() { return new UnixEventPort(); }
    void ueventPort_delete(UnixEventPort *ueventPort) { delete ueventPort; }
}

extern "C" {
    EventLoop *eventLoop_new(EventPort &port) { return new EventPort(port); }
    void eventLoop_delete(EventLoop *eventLoop) { delete eventLoop; }
}

extern "C" {
    WaitScope *waitScope_new(EventLoop *loop) { return new WaitScope(loop) }
    void waitScope_delete(WaitScope *waitScope) { delete waitScope; }
}

extern "C" {
    LowLevelAsyncIoProvider *ll_io_provider_new() { return new LowLevelAsyncIoProvider(); }
    void ll_io_provider_delete(LowLevelAsyncIoProvider *ll_io_provider) { delete ll_io_provider; }
}

extern "C" {
    AsyncIoProviderImpl *io_provider_implementation_new(LowLevelAsyncIoProvider *lowLevel) {    return new AsyncIoProviderImpl(lowLevel);}

    void io_provider_implementation_delete( AsyncIoProviderImpl *io_provider_implementation) { delete io_provider_implementation; }
}

extern "C" {
    TwoPartyServer *tp_server_new(ExecutionManagerServer *emServer, MsmHandler *msm_handler) { return new TwoPartyServer(emServer, msm_handler);}
    void tp_server_delete(TwoPartyServer *tp_server) { delete tp_server; }

}

extern "C" {
    const char *msmAddress = "unix:" + "/tmp/machine_management";
    const char *componentAddress = "unix:" + "/tmp/component_management";
    ExecutionManagerClient *em_client_new(const char *msmAddress, // "unix:"+"/tmp/machine_management"
                                      const char *componentAddress,     //  "unix:"+"/tmp/component_management"
                                      AsyncIoContext &context) {  return new ExecutionManagerClient(msmAddress, m_componentAddress, io_context); }

     void em_client_delete(ExecutionManagerClient *em_client) { delete em_client; }
     void em_confirm(ExecutionManagerClient *em_client, StateError) { em_client->confirm(StateError); }
}

extern "C" {
    ManifestReader *mr_new() { return new ManifestReader(); }
    void mr_delete(ManifestReader *mr) { delete mr; }
}



extern "C" {
    MsmHandler *msm_handler_new() { return new MsmHandler() } ;
    void em_delete(MsmHandler *msm_handler) { delete msm_handler; }
}

extern "C" {
    ExecutionManagerServer *emServer_new(ExecutionManager *em, MsmHandler *msm_handler) { return new ExecutionManagerServer(em, msm_handler);}
    void emServer_delete(ExecutionManagerServer *emServer) { delete emServer; }
    std::atomic<bool> emServer_isRunning(ExecutionManagerServer *emServer) { emServer->isRunning(); }
}

