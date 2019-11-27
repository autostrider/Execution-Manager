#ifndef SOCKET_SERVER_HPP
#define SOCKET_SERVER_HPP

#include <i_socket_server.hpp>

#include <memory>
#include <future>
#include <thread>
#include <sys/un.h>

class ISocketInterface;

namespace MSM
{

class SocketServer final: public ISocketServer
{
public:
  SocketServer(std::unique_ptr<ISocketInterface> socket, const std::string& path);
  std::string recv() override;
  void closeServer() override;
  void startServer() override;
private:
  void dataListener();
private:
  std::unique_ptr<ISocketInterface> m_socket;
  std::promise<std::string> m_newState;
  bool m_isAlive;
  int m_socketfd;
  struct sockaddr_un m_serverAddress;
  std::thread m_worker;
};

}

#endif // SOCKET_SERVER_HPP
