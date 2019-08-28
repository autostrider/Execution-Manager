#ifndef _APP_STATE_EVENTS__
#define _APP_STATE_EVENTS__

#include <tinyfsm.hpp>

struct kInitializing : tinyfsm::Event{};

struct kRunning : tinyfsm::Event{};

struct kTerminating : tinyfsm::Event{};

#endif
