#ifndef SOCKET_SERVER_HPP
#define SOCKET_SERVER_HPP

#include <i_socket_server.hpp>
#include <safe_queue.hpp>

#include <atomic>
#include <memory>
#include <future>
#include <sys/un.h>

using namespace common;

namespace api{
class ISocketInterface;
}

namespace MSM
{

class SocketServer final: public api::ISocketServer
{
public:
  SocketServer(std::unique_ptr<api::ISocketInterface> socket, const std::string& path);
  std::string getData() override;
  void closeServer() override;
  void startServer() override;

  ~SocketServer() override;
private:
  void dataListener();
private:
  std::unique_ptr<api::ISocketInterface> m_socket;
  std::atomic<bool> m_isAlive;
  SafeQueue m_receivedData;
  int m_socketfd;
  std::future<std::string> m_newState;
  struct sockaddr_un m_serverAddress;
  std::thread m_worker;
};

}

#endif // SOCKET_SERVER_HPP
