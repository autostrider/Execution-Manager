#include "lib_em_test.cpp";

extern "C"{
struct ExecutionManagerWrapper;
typedef struct ExecutionManagerWrapper ExecutionManagerWrapper;

       ExecutionManager* em_new(){
       ManifestReader* mr = mr_new();
       OsInterface* os = os_new();
       ApplicationHandler* ah = ah_new(os);
       AsyncIoContext* io_context = io_context_new();
       io_context->io_setupAsyncIo();
       ExecutionManagerClient* em_client = em_client_new("unix:"+"/tmp/machine_management","unix:"+"/tmp/component_management",io_context);

       return new ExecutionManager(mr,ah,em_client);
       }

       void em_delete(ExecutionManager* em) { delete em;}

       StateError em_setMachineState(ExecutionManager* em, char* str) { em->setMachineState(str);}
}