#include "socket_server.hpp"
#include <logger.hpp>
#include <i_socket_interface.hpp>

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

namespace MachineStateManager
{

SocketServer::SocketServer
(std::unique_ptr<ISocketInterface> socket, const std::string& path)
  : m_socket{std::move(socket)}
{
  struct sockaddr_un servAddr;
  servAddr.sun_family = AF_UNIX;
  path.copy(servAddr.sun_path, path.length());

  m_socketfd = m_socket->socket(AF_UNIX, SOCK_STREAM, 0);
  if (m_socketfd)
  {
    LOG << "Error opening socket";
  }

  int res = bind(m_socketfd, (struct sockaddr*)&servAddr, sizeof(servAddr));

  if (-1 == res) 
  {
    LOG << "Error binding data";
  }
  auto promise = std::promise<std::string>();
  auto data = promise.get_future();
  m_worker = std::thread{dataListener, std::move(promise), m_socketfd, std::move(m_socket)};
}

void SocketServer::dataListener(
    std::promise<std::string> newState, 
    int socketfd, 
    std::unique_ptr<ISocketInterface> socket)
{
  socket->listen(socketfd, 1);
  struct sockaddr_un cli;
  unsigned int cliSize =  sizeof(cli);
  socket->accept(socketfd, (struct sockaddr*)&cli, &cliSize);

  LOG << "State receiver server started...";

 constexpr int MAX = 128;
 char buff[MAX];
  for (;;) 
  { 
      bzero(buff, MAX); 
      read(socketfd, buff, sizeof(buff));

      newState.set_value(std::string{buff});
  }
}

std::string SocketServer::recv()
{
  return "";
}

} // namespace MachineStateManager