@0xd020a01856bb8bcf;

interface ApplicationStateManagement
{

  enum ApplicationState
  {
    kInitializing @0;
    kRunning @1;
    kShuttingdown @2;
  }

  reportApplicationState @0 (pid :Int32, state :ApplicationState);
}