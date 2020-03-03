#ifndef I_SOCKET_HPP
#define I_SOCKET_HPP

#include <sys/socket.h>
#include <unistd.h>


    class ISocket {
    public:
        virtual ~ISocket() = default;

        virtual int socket(int domain, int type, int protocol) const = 0;

        virtual int close(int socket_fd) const = 0;

    };


#endif //I_SOCKET_HPP
