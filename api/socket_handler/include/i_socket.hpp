#ifndef EXECUTION_MANAGER_I_SOCKET_HPP
#define EXECUTION_MANAGER_I_SOCKET_HPP
#include <sys/socket.h>

class ISocket {
public:
    virtual ~ISocket() = default;

    virtual int socket(int domain, int type, int protocol) const = 0;

    virtual int close(int socket_fd) const = 0;

};

#endif //EXECUTION_MANAGER_I_SOCKET_HPP
