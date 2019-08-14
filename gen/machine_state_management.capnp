@0xa6c7c6f7aabe643e;

interface MachineStateManagement 
{

  enum StateError 
  {
    kSuccess @0;
    kInvalidState @1;
    kInvalidRequest @2;
    kTimeout @3;
  }

  struct MachineStateResult 
  {
    result @0 :StateError;
    state @1 :Text; 
  }

  register @0 (app_name :Text, timeout :UInt32) 
    -> (result :StateError);

  getMachineState @1 (timeout :UInt32) 
    -> (result :MachineStateResult);

  setMachineState @2 (state :Text, timeout :UInt32) 
    -> (result :StateError);
    
}