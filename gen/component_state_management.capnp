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

    registerComponent @0 (component :Text);

    getComponentState @1 (component :Text)
      -> (state :Text, result :ComponentClientReturnType);

    confirmComponentState @2 (component :Text,
                              state :Text,
                              status :ComponentClientReturnType);
  }