#include "../include/server.h"

// init the server
Server::Server() {
    // show tips and info
    std::cout << "EzTransferer Server started... Press \"Ctrl-C\" to quit the program." << std::endl;

    // configure the server
    this->SetPort();
    this->SetRepositary();
    this->SetLog();
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
    // flag to check if the port is set
    bool port_set = false;

    while (!port_set) {
        std::cout << "Please enter the port number for the server[1024-65535](*Required): ";
        std::cin >> this->port;

        // check if the port is valid
        if (this->port < 1024 || this->port > 65535) {
            std::cout << "Invalid port number, please enter a number between 1024 and 65535." << std::endl;
        } else {
            std::cout << "Port set to " << this->port << std::endl;
            port_set = true;
        } 
    }
}

// set the repository
void Server::SetRepositary() {
    // flag to check if the repository is set
    bool repository_set = false;

    while (!repository_set) {
        std::cout << "Please enter the path of directory for the repository(*Required): ";
        std::string repository_path;
        std::cin >> repository_path;

        // check if the directory exists
        DIR* dir = opendir(repository_path.c_str());
        if (!dir) {
            std::cout << "Directory does not exist, please enter a valid directory." << std::endl;
            continue;
        }

        // check if the directory is writable
        if (access(repository_path.c_str(), W_OK) != 0) {
            std::cout << "Directory is not writable, please enter a directory with write permission." << std::endl;
            continue;
        }

        // check again whether to use the path
        std::cout << "Do you want to use the path " << repository_path << " for the repository? (y/n): ";
        std::string repository_input;
        std::cin >> repository_input;

        // cancel the use of the file path
        if (!(repository_input == "y" || repository_input == "Y")) continue;

        // set the repository
        this->repositary = dir;
        repository_set = true;
        std::cout << "Repository set to " << repository_path << std::endl;
    }
}

// set the log file
void Server::SetLog() {
    // check if the log file is neeeded
    std::cout << "Do you want to enable the log file? (y/n): ";
    std::string log_input;
    std::cin >> log_input;

    // disable the log file
    if (!(log_input == "y" || log_input == "Y")) {
        std::cout << "Log file disabled." << std::endl;
        return;
    }

    // set the log file path
    bool log_file_set = false;

    while (!log_file_set) {
        std::string log_file_path;
        std::cout << "Please enter the path of directory for the log file: ";
        std::cin >> log_file_path;

        // check if the directory exists
        if (!opendir(log_file_path.c_str())) {
            std::cout << "Directory does not exist, please enter a valid directory." << std::endl;
            continue;
        }

        // check if the directory is writable
        if (access(log_file_path.c_str(), W_OK) != 0) {
            std::cout << "Directory is not writable, please enter a directory with write permission." << std::endl;
            continue;
        }

        // check again whether to use the path
        std::cout << "Do you want to use the path " << log_file_path << " for the log file? (y/n): ";
        std::cin >> log_input;

        // cancel the use of the file path
        if (!(log_input == "y" || log_input == "Y")) continue;

        // create or open the log file
        this->log_file.open(log_file_path + "/eztransferer.log", std::ios::out | std::ios::app);
        if (!this->log_file.is_open()) {
            std::cout << "Failed to open the log file, please enter a valid directory." << std::endl;
            continue;
        }
        log_file_set = true;
        std::cout << "Log file set to " << log_file_path << "/eztransferer.log" << std::endl;
    }
}

// output log
void Server::OutputLog(std::string message) {
    // get the current time
    std::stringstream ss;
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    ss << std::put_time(std::localtime(&time), "%F %T: ");

    // add time info
    message = ss.str() + message;

    // output the log message
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

