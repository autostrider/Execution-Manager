#ifndef EXECUTION_MANAGER_I_SERVER_HPP
#define EXECUTION_MANAGER_I_SERVER_HPP


    class IServer {
    public:
        virtual ~IServer() = default;

        virtual bool start() = 0;

        virtual void stop() = 0;
    };

#endif //EXECUTION_MANAGER_I_SERVER_HPP
