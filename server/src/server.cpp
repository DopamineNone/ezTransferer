#include "../include/server.h"

// init the server
Server::Server() {
    // show tips and info
    std::cout << "EzTransferer Server started... Press \"Ctrl-C\" to quit the program." << std::endl;

    // configure the server
    this->SetPort();
    this->SetRepositary();
    this->SetLog();
    this->OutputLog("Finished configuring the server!");
}

//close the server
Server::~Server() {
    // close the socket
    close(this->sockfd);

    // close the repository
    closedir(this->repositary);

    // close log file
    if (this->log_file.is_open()) {
        this->log_file.close();
    }
    this->OutputLog("Server stopped.");
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
    // init the socket
    this->sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (this->sockfd < 0) {
        this->OutputLog("Failed to create socket.");
        this->Stop();
    }

    // bind the socket
    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(this->port);
    if (bind(this->sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        this->OutputLog("Failed to bind socket.");
        this->Stop();
    }

    // listen the socket
    if (listen(this->sockfd, 10) < 0) {
        this->OutputLog("Failed to listen socket.");
        this->Stop();
    }

    while (true) {
        // accept the connection
        sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);
        int client_sockfd = accept(this->sockfd, (struct sockaddr*)&client_addr, &client_addr_len);
        if (client_sockfd < 0) {
            // failed, close the socket
            this->OutputLog("Failed to accept connection.");
            close(client_sockfd);
        } else {
            // success, create a new thread to handle the request
            std::string client_info = std::string(inet_ntoa(client_addr.sin_addr))+":"+std::to_string(ntohs(client_addr.sin_port));
            this->OutputLog("New connection from " + client_info);
            std::thread new_thread(&Server::HandleRequest, this, client_sockfd, client_info);
            new_thread.detach();
        }
    }
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
void Server::HandleRequest(int client_sockfd, std::string client_info) {
    char buffer[MAX_BUFFER_SIZE];
    Request request;
    ssize_t recv_bytes = recv(client_sockfd, buffer, MAX_BUFFER_SIZE, 0);

    // check if the request is received
    if (recv_bytes <= 0) {
        this->OutputLog("Failed to receive request from " + client_info + ".");
        this->ReportError(client_sockfd, client_info, "Failed to receive request.");
    } else {
        try
        {
            // parse the request
            memcpy(&request, buffer, sizeof(Request));
            this->OutputLog("Parsed request from " + client_info + " with operation code " + std::to_string(request.op));

            // handle the request
            switch (request.op)
            {
            case FETCH_FILE:
                this->SendFile();
                break;
            case VIEW_DIRECTORY:
                this->ListFiles();
                break;
            default:
                this->OutputLog("Received invalid operation code.");
                this->ReportError(client_sockfd, client_info, "Invalid operation code.");
                break;
            }
        }
        catch(const std::exception& e)
        {
            this->OutputLog("Failed to parse request from " + client_info + ".");

        }
    }

    // close the socket
    close(client_sockfd);

}

// list files
void Server::ListFiles() {

}

// send file
void Server::SendFile() {

}

// report error to client
void Server::ReportError(int client_sockfd, std::string client_info, std::string message = "") {
    char buffer[MAX_BUFFER_SIZE];

    // resize message size
    if (message.size() > MAX_BUFFER_SIZE) {
        std::cout << "Error message is too long, truncated to " << MAX_BUFFER_SIZE << " characters." << std::endl;
        message.resize(MAX_BUFFER_SIZE);
    }

    // create the response
    MarshalResponse(buffer, FAILED, message.length(), message.c_str());
    
    // send the response
    int send_bytes = send(client_sockfd, buffer, sizeof(Response), 0);
    if (send_bytes <= 0) {
        this->OutputLog("Failed to send error message to client: " + client_info + ".");
    } else {
        this->OutputLog("Error message was sent to client: " + client_info + ".");
    }
}   

