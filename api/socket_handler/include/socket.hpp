#ifndef EXECUTION_MANAGER_SOCKET_HPP
#define EXECUTION_MANAGER_SOCKET_HPP

#include <string>

class Socket
{
private:
    bool fd_created;
protected:
    int fd, temp_socket;
    bool isCreated();

public:
    Socket();
    ~Socket();
   // void close();
};
#endif //EXECUTION_MANAGER_SOCKET_HPP
