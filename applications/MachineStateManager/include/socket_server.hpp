#ifndef SOCKET_SERVER_HPP
#define SOCKET_SERVER_HPP

#include <i_socket_server.hpp>

#include <atomic>
#include <memory>
#include <future>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <queue>
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

  ~SocketServer() override;
private:
  void dataListener();
private:
  std::unique_ptr<ISocketInterface> m_socket;
  std::atomic<bool> m_isAlive;
  std::queue<std::string> m_dataReceived;
  std::mutex m_mut;
  std::condition_variable m_condVar;
  int m_socketfd;
  std::future<std::string> m_newState;
  struct sockaddr_un m_serverAddress;
  std::thread m_worker;
};

}

#endif // SOCKET_SERVER_HPP
