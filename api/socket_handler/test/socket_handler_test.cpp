//
// Created by yavorann on 27.01.20.
//
#include <iostream>
#include <string>
#include "../include/socket_server.hpp"
#include "../include/socket_client.hpp"

using namespace std;
int main() {
    std::string temp("Client string");

    SocketClient csocket("/path/to/socket");
    if (csocket.isReady()) {

        csocket.connect();
        if (csocket.isConnected()) {
            csocket << temp;
            csocket >> temp;
            cout << "to server: " << temp << endl;
        }
    }


    std::string msg("string from server");

    SocketServer ssocket("/PATH/TO/SOCKET");
    if (ssocket.isConnected()) {

        if (ssocket.accept()) {

            ssocket >> msg;
            cout << "resp to client: " << msg << endl;
            msg = "hi server";
            ssocket << msg;
        }
    }

    return 0;
}