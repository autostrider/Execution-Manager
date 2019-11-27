#include "socket_server.hpp"
#include <logger.hpp>
#include <i_socket_interface.hpp>

#include <sys/socket.h>

namespace MSM
{

SocketServer::SocketServer
(std::unique_ptr<ISocketInterface> socket, const std::string& path)
  : m_socket{std::move(socket)},
    m_isAlive{true},
    m_socketfd{m_socket->socket(AF_UNIX, SOCK_STREAM, 0)},
    m_newState{},
    m_serverAddress{},
    m_worker{}
{
  ::unlink(path.c_str());
  m_serverAddress.sun_family = AF_UNIX;
  path.copy(m_serverAddress.sun_path, path.length());

  if (m_socketfd)
  {
    LOG << "Error opening socket";
  }
}

void SocketServer::dataListener()
{
  struct sockaddr_un cli;
  unsigned int cliSize =  sizeof(cli);

  LOG << "State receiver server started...";

  constexpr int BUFFER_SIZE = 128;
  char buff[BUFFER_SIZE];
  do
  {
    LOG << "New promise set";
    auto newStatePromise = std::promise<std::string>();
    m_newState = newStatePromise.get_future();
    LOG << "before accept";
    int resfd = m_socket->accept(m_socketfd, (struct sockaddr*)&cli, &cliSize);
    bzero(buff, BUFFER_SIZE);
    m_socket->recv(resfd, buff, sizeof(buff), 0);
    newStatePromise.set_value(std::string{buff});
    LOG << "data was set";
  } while (m_isAlive);
}

std::string SocketServer::recv()
{
  LOG << "before";
  while (!m_newState.valid()) {

  }
  LOG << "here";
  return m_newState.get();
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
  m_socket->listen(m_socketfd, 5);

  m_worker = std::thread(&SocketServer::dataListener, this);
}

SocketServer::~SocketServer()
{
  if (m_worker.joinable())
  {
    m_worker.join();
  }

  close(m_socketfd);
}

} // namespace MachineStateManager
