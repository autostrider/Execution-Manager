#ifndef EXECUTION_MANAGER_I_SOCKET_SERVER_HPP
#define EXECUTION_MANAGER_I_SOCKET_SERVER_HPP

#include <string>

namespace api
{

class ISocketServer
{
public:
  virtual std::string getData() = 0;
  virtual void startServer() = 0;
  virtual void closeServer() = 0;
  virtual ~ISocketServer() = default;
};

}

#endif // EXECUTION_MANAGER_I_SOCKET_SERVER_HPP
