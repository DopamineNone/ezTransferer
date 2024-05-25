#include "../include/server.h"

// init the server
Server::Server() {
    // 
}

//close the server
Server::~Server() {
    // close log file
    if (this->log_file.is_open()) {
        this->log_file.close();
    }
    std::cout << "Server closed..." << std::endl;
}

// call the constructor of Server
Server* Server::NewServer() {
    // create a new server
    Server* server = new Server();

    // add the server to the list
    Server::server_list.push_back(server);

    return server;
}

// stop all active servers
void Server::StopAllServices(int signum) {
    // Ctrl-C quit the program
    if (signum == 2) {
        while (!Server::server_list.empty()) {
            Server* server = Server::server_list.back();
            server->Stop();
            Server::server_list.pop_back();
        }
    }
}

// run the server
void Server::Run() {

}

// stop server
void Server::Stop() {
    delete this;
}

// set the port
void Server::SetPort() {

}

// set the log file
void Server::SetLog() {
    
}

// set the repository
void Server::SetRepositary() {

}

// output log
void Server::OutputLog(std::string message) {
    if (this->log_file.is_open()) {
        this->log_file << message << std::endl;
    }
    std::cout << message << std::endl;
}

// handle request
void Server::HandleRequest() {

}

// list files
void Server::ListFiles() {

}

// send file
void Server::SendFile() {

}

