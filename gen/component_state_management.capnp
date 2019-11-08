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

    enum StateUpdateMode
    {
      kPoll @0;
      kEvent @1;
    }

    registerComponent @0 (component :Text, mode :StateUpdateMode);

    getComponentState @1 (component :Text)
      -> (state :Text, result :ComponentClientReturnType);

    confirmComponentState @2 (component :Text,
                              state :Text,
                              status :ComponentClientReturnType);

    interface StateManager
    {
      setComponentState @0 (state :Text)
        -> (result :ComponentClientReturnType);
    }
  }