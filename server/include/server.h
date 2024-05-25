#ifndef _EZTRANSFERER_SERVER_H
#define _EZTRANSFERER_SERVER_H

#include<stdio.h>
#include<iostream>
#include<cstring>
#include<sys/fcntl.h>
#include<sys/socket.h>
#include<unistd.h>
#include<netinet/in.h>
#include<errno.h>
#include<sys/types.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fstream>
#include <string>
#include <thread>
#include "../../api/ezTransferer.h"
#include <string.h>
#include <csignal>
#include <vector>

class Server {
    private:
        // socket that maintain the service
        int sockfd;

        // log file handler
        std::ofstream log_file;

        // repositary handler
        DIR* repositary;

        // active server list
        static std::vector<Server*> server_list;

        // initialize the server
        Server();
    public:
        static Server* NewServer();
        // run the server
        void Run();

        // handle the request
        void HandleRequest();

        // list the files in the repositary
        void ListFiles();

        // send the file
        void SendFile();

        // set the port number
        void SetPort();
        
        // set the log file
        void SetLog();
        
        // set the repositary
        void SetRepositary();

        // output log
        void OutputLog(std::string message);

        // stop the server
        static void Stop(int signum);

        // close the server
        ~Server();
};


#endif