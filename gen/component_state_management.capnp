  @0xd629d57019d2629d;

  interface StateManagement
  {
    enum ComponentClientReturnType
    {
      kSuccess @0;
      kGeneralError @1;
      kPending @2;
      kInvalid @3;
      kUnchanged @4;
    }

    getComponentState @0 (pid :Int32)
      -> (state :Text, result :ComponentClientReturnType);

    confirmComponentState @1 (pid :Int32,
                              state :Text,
                              status :ComponentClientReturnType);
  }