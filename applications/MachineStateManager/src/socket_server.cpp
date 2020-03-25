#include "socket_server.hpp"
#include <logger.hpp>
#include <i_socket_interface.hpp>

#include <sys/socket.h>
#include <stdexcept>
#include <exception>

namespace MSM
{

SocketServer::SocketServer
(std::unique_ptr<api::ISocketInterface> socket, const std::string& path)
  : m_socket{std::move(socket)},
    m_isAlive{true},
    m_receivedData{},
    m_socketfd{m_socket->socket(AF_UNIX, SOCK_STREAM, 0)},
    m_newState{},
    m_serverAddress{},
    m_worker{}
{
  ::unlink(path.c_str());
  m_serverAddress.sun_family = AF_UNIX;
  path.copy(m_serverAddress.sun_path, path.length());

  if (m_socketfd < 0)
  {
     LOG << "Error opening socket " << strerror(errno);
  }

  int res = m_socket->bind(m_socketfd,
                 (struct sockaddr*)&m_serverAddress,
                 sizeof(m_serverAddress));

  if (-1 == res)
  {
    LOG << "Error binding data";
  }

  res = m_socket->listen(m_socketfd, 5);

  if (-1 == res)
  {
    LOG << "Error listening in socket";
  }
}

void SocketServer::dataListener()
{
  struct sockaddr_un cli;
  unsigned int cliSize =  sizeof(cli);
  constexpr int BUFFER_SIZE = 100000;

  LOG << "State receiver server started...";

  char buff[BUFFER_SIZE];
  int clientfd = m_socket->accept(m_socketfd, (struct sockaddr*)&cli, &cliSize);
  LOG << "Client connected successfully";
  do
  {
    bzero(buff, BUFFER_SIZE);
    auto resRecv = m_socket->recv(clientfd, buff, BUFFER_SIZE, 0);
    if (resRecv <= 0)
    {
      LOG << "Error occurred receiving data from client";
      break;
    }
    else
    {
      std::string res{buff};
      m_receivedData.push(res);
    }
  } while (m_isAlive);
  m_socket->close(clientfd);
}

std::string SocketServer::getData()
{
  return m_receivedData.pop();
}

void SocketServer::closeServer()
{
    m_isAlive = false;
}

void SocketServer::startServer()
{
  m_worker = std::thread(&SocketServer::dataListener, this);
}

SocketServer::~SocketServer()
{
  if (m_worker.joinable())
  {
    m_worker.join();
  }
  LOG << "closing server on: " << m_serverAddress.sun_path;
  m_socket->close(m_socketfd);
  ::unlink(m_serverAddress.sun_path);
}

} // namespace MachineStateManager
