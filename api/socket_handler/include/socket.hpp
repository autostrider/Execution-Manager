#ifndef EXECUTION_MANAGER_SOCKET_HPP
#define EXECUTION_MANAGER_SOCKET_HPP

#include <string>
#include <logger.hpp>

class Socket
{
private:
    bool fd_created;
protected:
    int fd;
    bool isCreated();

public:
    Socket();
    ~Socket();
};
#endif //EXECUTION_MANAGER_SOCKET_HPP
