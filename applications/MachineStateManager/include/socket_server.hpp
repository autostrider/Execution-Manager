#ifndef SOCKET_SERVER_HPP
#define SOCKET_SERVER_HPP

#include <i_socket_server.hpp>

#include <memory>
#include <future>
#include <thread>

class ISocketInterface;

namespace MachineStateManager
{

class SocketServer final: public ISocketServer
{
public:
  SocketServer(std::unique_ptr<ISocketInterface> socket, const std::string& path);
  std::string recv() override;
private:
  void dataListener(
    std::promise<std::string> newState, 
    int socketfd,
    std::unique_ptr<ISocketInterface> socket
  );
private:
  std::unique_ptr<ISocketInterface> m_socket;
  int m_socketfd;
  std::thread m_worker;
};

}

#endif // SOCKET_SERVER_HPP