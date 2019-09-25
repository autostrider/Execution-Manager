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

  register @0 (pid :Int32, appName :Text)
    -> (result :StateError);

  getMachineState @1 (pid :Int32)
    -> (state :Text, result :StateError);

  setMachineState @2 (pid :Int32, state :Text)
    -> (result :StateError);

  interface MachineStateManager
  {
    confirmStateTransition @0 (result :StateError);
  }
}