#include "../include/server.h"

// init the list of servers
std::vector<Server*> Server::server_list = std::vector<Server*>();

// init the server
Server::Server() {
    // show tips and info
    std::cout << "EzTransferer Server started... Press \"Ctrl-C\" to quit the program." << std::endl;

    // configure the server
    this->SetPort();
    this->SetRepository();
    this->SetLog();
    this->OutputLog("Finished configuring the server!");
}

//close the server
Server::~Server() {
    this->OutputLog("Server stopped.");

    // close the socket
    close(this->sockfd);

    // close the repository
    if (this->repository_path != nullptr) {
        delete this->repository_path;
    }

    // close log file
    if (this->log_file.is_open()) {
        this->log_file.close();
    }
}

// call the constructor of Server
Server* Server::NewServer() {
    // create a new server
    Server* server = new Server();

    // add the server to the list
    server_list.push_back(server);

    return server;
}

// stop all active servers
void Server::StopAllServices(int signum) {
    // Ctrl-C quit the program
    if (signum == SIGINT) {
        std::cout << "Stopping all servers..." << std::endl;
        while (!server_list.empty()) server_list[0]->Stop();
        exit(0);
    }
}

// run the server
void Server::Run() {
    // listen the socket
    if (listen(this->sockfd, 10) < 0) {
        this->OutputLog("Failed to listen socket.");
        this->Stop();
    }

    // start the server
    this->OutputLog("Server running...");
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
    // delete this from server_list
    for (int i = 0; i < server_list.size(); i++) {
        if (server_list[i] == this) {
            server_list.erase(server_list.begin() + i);
            this->OutputLog("Server " + std::to_string(i) + " stopped.");
            break;
        }
    }
    delete this;
}

// set the port
void Server::SetPort() {
    // flag to check if the port is set
    bool port_set = false;

    while (!port_set) {
        int port;
        std::cout << "Please enter the port number for the server[1024-65535](*Required): ";
        std::cin >> port;

        // check if the port is valid
        if (port < 1024 || port > 65535) {
            std::cout << "Invalid port number, please enter a number between 1024 and 65535." << std::endl;
            continue;
        } 

        // check if the port is available
        this->sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (this->sockfd < 0) {
            std::cout << "Failed to create socket, please try again." << std::endl;
            continue;
        }
        sockaddr_in test_addr;
        test_addr.sin_family = AF_INET;
        test_addr.sin_addr.s_addr = INADDR_ANY;
        test_addr.sin_port = htons(port);
        if (bind(this->sockfd, (struct sockaddr*)&test_addr, sizeof(test_addr)) < 0) {
            std::cout << "Port " << port << " is already in use, please enter a different port." << std::endl;
            close(this->sockfd);
            continue;
        }

        //check again whether to use the port
        std::cout << "Do you want to use the port " << port << " for the server? (y/n): ";
        std::string port_input;
        std::cin >> port_input;

        // cancel the use of the port
        if (!(port_input == "y" || port_input == "Y")) {
            close(this->sockfd);
            continue;
        }

        // set the port
        this->port = port;
        std::cout << "Port set to " << this->port << std::endl;
        port_set = true;
    }
}

// set the repository
void Server::SetRepository() {
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
        std::string input;
        std::cin >> input;

        // cancel the use of the file path
        if (!(input == "y" || input == "Y")) continue;

        // set the repository
        int n = repository_path.length() + 1;
        this->repository_path = new char[n];
        strncpy(this->repository_path, repository_path.c_str(), n);
        closedir(dir);
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
    char buffer[sizeof(Request)];
    ssize_t recv_bytes = recv(client_sockfd, buffer, sizeof(Request), 0);

    // check if the request is received
    if (recv_bytes <= 0) {
        this->OutputLog("Failed to receive request from " + client_info + ".");
        this->ReportError(client_sockfd, client_info, "Failed to receive request.");
    } else {
        try
        {
            // parse the request
            unsigned int op;
            char filename[MAX_FILENAME_SIZE];
            UnmarshalRequest(buffer, &op, filename);
            this->OutputLog("Parsed request from " + client_info + " with operation code " + std::to_string(op));

            // handle the request
            switch (op)
            {
            case FETCH_FILE:
                this->SendFile(client_sockfd, client_info, filename);
                break;
            case VIEW_DIRECTORY:
                this->ListFiles(client_sockfd, client_info);
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
void Server::ListFiles(int client_sockfd, std::string client_info) {
    // check if the repository is set
    DIR* dir;
    if (!(dir = opendir(this->repository_path))) {
        this->OutputLog("Failed to list files, repository is lost, server shut down.");
        this->ReportError(client_sockfd, client_info, "Repository is not set.");
        this->Stop();
        return;
    }

    // set file lists
    std::stringstream ss;
    dirent* entry;
    while ((entry = readdir(dir))) {
        // skip directories
        if (entry->d_type == DT_DIR) continue;

        // skip hidden files
        if (entry->d_name[0] == '.') continue;

        ss << entry->d_name << " ";  
    }

    // send the response
    // start transfer
    std::string file_list = ss.str();
    char buffer[sizeof(Response)];
    MarshalResponse(buffer, START_TRANSFER, file_list.length(), nullptr);
    int send_bytes = send(client_sockfd, buffer, sizeof(Response), 0);
    if (send_bytes <= 0) {
        this->OutputLog("Failed to send file list to " + client_info + ".");
        this->ReportError(client_sockfd, client_info, "Failed to send file list.");
        return;
    }
    this->OutputLog("Sending file list to " + client_info + ".");

    // transfer file list
    for (int i = 0; i < file_list.length(); i += MAX_BUFFER_SIZE) {
        std::string response = file_list.substr(i, MAX_BUFFER_SIZE);
        char* data = new char[response.length()];
        std::strncpy(data, response.c_str(), response.length());
        MarshalResponse(buffer, TRANSFERING, response.length(), data);
        delete[] data;
        send_bytes = send(client_sockfd, buffer, sizeof(Response), 0);
        if (send_bytes <= 0) {
            this->OutputLog("Failed to send file list to " + client_info + ".");
            this->ReportError(client_sockfd, client_info, "Failed to send file list.");
            return;
        }
    }

    // send the end of transfer message
    MarshalResponse(buffer, FINISHED_SUCCESS, 0, nullptr);
    send_bytes = send(client_sockfd, buffer, sizeof(Response), 0);
    if (send_bytes <= 0) {
        this->OutputLog("Failed to send file list end message to " + client_info + ".");
        this->ReportError(client_sockfd, client_info, "Failed to send file list end message.");
        return;
    }
    this->OutputLog("File list was sent to " + client_info + ".");
}

// send file
void Server::SendFile(int client_sockfd, std::string client_info, char* filename) {
    // check if the repository is set
    DIR* dir;
    if (!(dir = opendir(this->repository_path))) {
        this->OutputLog("Failed to list files, repository is lost, server shut down.");
        this->ReportError(client_sockfd, client_info, "Repository is not set.");
        this->Stop();
        return;
    }

    // filter malformed filename
    std::string file_name(filename);
    if (file_name.find('/') != std::string::npos || file_name.find('\\') != std::string::npos ||
    file_name[0] == '.' || file_name.empty() || std::count(file_name.begin(),file_name.end(), '.') > 1) {
        this->OutputLog("Received malformed filename from " + client_info + ".");
        this->ReportError(client_sockfd, client_info, "Malformed filename.");
        return;
    }

    // find the file
    // start transfer
    std::ifstream file(std::string(this->repository_path) + "/" + file_name, std::ios::binary);
    if (!file.is_open()) {
        this->OutputLog("Failed to open file " + file_name + " for reading.");
        this->ReportError(client_sockfd, client_info, "Non-existent file or permission denied to fetch the file.");
        return;
    }

    // set the file size
    file.seekg(0, std::ios::end);
    int file_size = static_cast<int>(file.tellg());
    file.seekg(0, std::ios::beg);
    this->OutputLog("Sending file " + file_name + " to " + client_info + ".");
    char buffer[sizeof(Response)];
    MarshalResponse(buffer, START_TRANSFER, file_size, nullptr);
    int send_bytes = send(client_sockfd, buffer, sizeof(Response), 0);
    if (send_bytes <= 0) {
        this->OutputLog("Failed to send file header to " + client_info + ".");
        this->ReportError(client_sockfd, client_info, "Failed to send file header.");
        return;
    }

    // transfer file
    while (!file.eof()) {
        char data[MAX_BUFFER_SIZE];
        file.read(data, MAX_BUFFER_SIZE);
        int read_bytes = file.gcount();
        if (read_bytes <= 0) break;
        MarshalResponse(buffer, TRANSFERING, read_bytes, data);
        send_bytes = send(client_sockfd, buffer, sizeof(Response), 0);
        if (send_bytes <= 0) {
            this->OutputLog("Failed to send file data to " + client_info + ".");
            this->ReportError(client_sockfd, client_info, "Failed to send file data.");
            file.close();
            return;
        }
    }

    // send the end of transfer message
    file.close();
    MarshalResponse(buffer, FINISHED_SUCCESS, 0, nullptr);
    send_bytes = send(client_sockfd, buffer, sizeof(Response), 0);
    if (send_bytes <= 0) {
        this->OutputLog("Failed to send file end message to " + client_info + ".");
        this->ReportError(client_sockfd, client_info, "Failed to send file end message.");
        return;
    }
    this->OutputLog("File " + file_name + " was sent to " + client_info + ".");

}

// report error to client
void Server::ReportError(int client_sockfd, std::string client_info, std::string message = "") {
    char buffer[sizeof(Response)];

    // resize message size
    if (message.size() > MAX_BUFFER_SIZE) {
        std::cout << "Error message is too long, truncated to " << MAX_BUFFER_SIZE << " characters." << std::endl;
        message.resize(MAX_BUFFER_SIZE);
    }

    // create the response
    char* data = new char[message.length()];
    std::strncpy(data, message.c_str(), message.length());
    MarshalResponse(buffer, FAILED, message.length(), data);
    delete[] data;

    // send the response
    int send_bytes = send(client_sockfd, buffer, sizeof(Response), 0);
    if (send_bytes <= 0) {
        this->OutputLog("Failed to send error message to client: " + client_info + ".");
    } else {
        this->OutputLog("Error message was sent to client: " + client_info + ".");
    }
}   
