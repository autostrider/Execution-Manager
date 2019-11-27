#ifndef I_SOCKET_SERVER_HPP
#define I_SOCKET_SERVER_HPP

#include <string>

class ISocketServer
{
public:
  virtual std::string recv() = 0;
  virtual ~ISocketServer() = default;
};

#endif // I_SOCKET_SERVER_HPP