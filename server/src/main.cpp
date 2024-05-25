#include <csignal>
#include "../include/server.h"

int main() {
    // register signal handler for SIGINT
    signal(SIGINT, Server::StopAllServices);
    // Init a server and run it
    Server* server = Server::NewServer();
    server->Run();
    return 0;
}

