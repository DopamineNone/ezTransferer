#include "../include/server.h"

// call the constructor of Server
Server* Server::NewServer() {
    return new Server();
}


// init the server
Server::Server() {
    // init signal handler
    

    // 
}

//close the server
Server::~Server() {
    if (this->log_file.is_open()) {
        this->log_file.close();
    }
    std::cout << "Server closed..." << std::endl;
}

// run the server
void Server::Run() {

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

// stop server
void Server::Stop(int signum) {

}