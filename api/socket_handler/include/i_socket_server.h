//
// Created by denys on 01.10.19.
//

#ifndef EXECUTION_MANAGER_I_SOCKET_SERVER_H
#define EXECUTION_MANAGER_I_SOCKET_SERVER_H

class ISocketServer {
public:
    virtual ~ISocketServer() = default;

    virtual bool run() = 0;
    virtual void stop() = 0;
};


#endif //EXECUTION_MANAGER_I_SOCKET_SERVER_H
