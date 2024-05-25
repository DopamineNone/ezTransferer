#ifndef _EZTRANSFERER_SERVER_H
#define _EZTRANSFERER_SERVER_H


#include "../../api/ezTransferer.h"
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
#include <string.h>
#include <csignal>
#include <vector>
#include <iomanip>
#include <chrono>
#include <sstream>

class Server {
    private:
        // socket that maintain the service
        int sockfd;

        // port number
        int port;

        // log file handler
        std::ofstream log_file;

        // repositary handler
        DIR* repositary;

        // active server list
        static std::vector<Server*> server_list;

        // initialize the server
        Server();
    public:
        // close the server
        ~Server();

        // create a new server
        static Server* NewServer();

        // stop all active servers
        static void StopAllServices(int signum);

        // run the server
        void Run();

        // stop the server
        void Stop();

        // handle the request
        void HandleRequest();

        // list the files in the repositary
        void ListFiles();

        // send the file
        void SendFile();

        // set the port number
        void SetPort();

        // set the repositary
        void SetRepositary();

        // set the log file
        void SetLog();

        // output log
        void OutputLog(std::string message);

};


#endif