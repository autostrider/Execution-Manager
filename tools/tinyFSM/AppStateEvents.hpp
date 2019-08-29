#ifndef _APP_STATE_EVENTS__
#define _APP_STATE_EVENTS__

#include <tinyfsm.hpp>

struct Initializing : tinyfsm::Event{};

struct Running : tinyfsm::Event{};

struct Terminating : tinyfsm::Event{};

#endif
