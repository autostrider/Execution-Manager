#ifndef EXECUTION_MANAGER_SOCKET_HPP
#define EXECUTION_MANAGER_SOCKET_HPP

#include <string>
#include <logger.hpp>

class Socket
{
private:
    bool m_fdcreated;
protected:
    int m_fd;
    bool isCreated();

public:
    Socket();
    ~Socket();
};
#endif //EXECUTION_MANAGER_SOCKET_HPP
