#include "execution_management_api.hpp"

int hello()
{
  ::capnp::MallocMessageBuilder message;

  Person::Builder alice = message.initRoot<Person>();

  alice.setId(123);
  alice.setName("Alice");
  alice.setEmail("alice@example.com");

  return alice.getId();
}