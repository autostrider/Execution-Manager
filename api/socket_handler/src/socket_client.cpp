#include "../include/socket_client.hpp"
#include <logger.hpp>
#include <string>
#include <fcntl.h>

SocketClient::SocketClient(const std::string& path) : Socket(), ready(false), connected(false) {
    assignSocket(path);
}

void SocketClient::assignSocket(const std::string& path)
{

  if (isOk()) {
      addr_un.sun_family = AF_UNIX;
      path.copy(addr_un.sun_path, path.length());

      if (fd < 0) {
          LOG << "Error opening socket " << strerror(errno);
      }
    //THIS CHECK DOESN'T PASS
      if (::bind(fd,
              (struct sockaddr *) &addr_un,
              sizeof(addr_un))==-1) {
          LOG << "Error binding data";
      }
      ready = true;
  }
}


void SocketClient::connect(){
    //THIS CHECK ALSO DOESN'T PASS
  if (::connect(fd, ( const struct sockaddr*) &addr_un, sizeof(addr_un))!= -1) {
    temp_socket = fd;
    connected=true;
  }
  else{
      LOG<<"Client failed to connect socket";
  }
}

bool SocketClient::isReady()
{
  return ready;
}

bool SocketClient::isConnected()
{
  return connected;
}

