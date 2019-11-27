#include "socket_server.hpp"
#include <logger.hpp>
#include <i_socket_interface.hpp>

#include <sys/socket.h>

namespace MSM
{

SocketServer::SocketServer
(std::unique_ptr<ISocketInterface> socket, const std::string& path)
  : m_socket{std::move(socket)},
    m_newState{std::promise<std::string>()},
    m_isAlive{true},
    m_socketfd{m_socket->socket(AF_UNIX, SOCK_STREAM, 0)},
    m_serverAddress{},
    m_worker{}
{
  m_serverAddress.sun_family = AF_UNIX;
  path.copy(m_serverAddress.sun_path, path.length());

  if (m_socketfd)
  {
    LOG << "Error opening socket";
  }
}

void SocketServer::dataListener()
{
  m_socket->listen(m_socketfd, 1);
  struct sockaddr_un cli;
  unsigned int cliSize =  sizeof(cli);
  m_socket->accept(m_socketfd, (struct sockaddr*)&cli, &cliSize);

  LOG << "State receiver server started...";

  constexpr int BUFFER_SIZE = 128;
  char buff[BUFFER_SIZE];
  do
  {
      m_newState = std::promise<std::string>();
      bzero(buff, BUFFER_SIZE);
      read(m_socketfd, buff, sizeof(buff));
      m_newState.set_value(std::string{buff});
  } while (m_isAlive);
}

std::string SocketServer::recv()
{
  auto result = m_newState.get_future();
  return result.get();
}

void SocketServer::closeServer()
{
    m_isAlive = false;
}

void SocketServer::startServer()
{
  int res = bind(m_socketfd, (struct sockaddr*)&m_serverAddress, sizeof(m_serverAddress));

  if (-1 == res)
  {
    LOG << "Error binding data";
  }

  m_worker = std::thread(&SocketServer::dataListener, this);
}

} // namespace MachineStateManager
