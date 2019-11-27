#include "socket_server.hpp"
#include <logger.hpp>
#include <i_socket_interface.hpp>

#include <sys/socket.h>
#include <stdexcept>
#include <exception>

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
  m_serverAddress.sun_family = AF_UNIX;
  path.copy(m_serverAddress.sun_path, path.length());

  if (m_socketfd < 0)
  {
     LOG << "Error opening socket " << strerror(errno);
  }
}

void SocketServer::dataListener()
{
  struct sockaddr_un cli;
  unsigned int cliSize =  sizeof(cli);

  LOG << "State receiver server started...";

  constexpr int BUFFER_SIZE = 128;
  char buff[BUFFER_SIZE];
  int resfd = m_socket->accept(m_socketfd, (struct sockaddr*)&cli, &cliSize);
  LOG << "Client connected successfully";
  do
  {
    bzero(buff, BUFFER_SIZE);
    auto newStatePromise = std::promise<std::string>();
    m_newState = newStatePromise.get_future();
    int resRecv = m_socket->recv(resfd, buff, BUFFER_SIZE, 0);
    if (resRecv <= 0)
    {
      try
      {
        throw std::runtime_error("Connection closed");
      }
      catch (...)
      {
        newStatePromise.set_exception(std::current_exception());
      }
      break;
    }
    newStatePromise.set_value(std::string{buff});
  } while (m_isAlive);
}

std::string SocketServer::recv()
{
  while (!m_newState.valid())
  { }
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
  ::unlink(m_serverAddress.sun_path);
  close(m_socketfd);
}

} // namespace MachineStateManager
