#include<stdio.h>
#include<stdlib.h>
#include<WinSock2.h>
#include <Windows.h>

#include"..\\api\\src\\ezTransferer.c"

#pragma comment(lib,"ws2_32.lib")

#define PORT 8080
#define BUFFER_SIZE 1024

void fetch(SOCKET);
void intro();
void view(SOCKET);
void updateProgress(int);


int main() {
    char ins;

    printf("//////ezTransferer//////\n");

    WSADATA wsa;
    SOCKET client_socket;
    struct sockaddr_in server;
    
    char server_ip[16];

    if (WSAStartup(MAKEWORD(2,2), &wsa) != 0) {
        printf("WSAStartup failed.\n");
        return 1;
    }

    printf("Enter the server IP address: ");
    fgets(server_ip, sizeof(server_ip), stdin);
    server_ip[strcspn(server_ip, "\n")] = 0;
    
    
    // server config
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(server_ip);
    server.sin_port = htons(PORT);

    while(1) {
        printf("Enter An Instruction:(\"h\"for Help)\n");
        scanf(" %c",&ins);
        fflush(stdin);
        if (ins == 'e') break;

        if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
            printf("Socket creation failed.\n");
            return 1;
        }
        if (connect(client_socket, (struct sockaddr *)&server, sizeof(server)) < 0) {
            printf("Connection failed.\n");
            return 1;
        }
        switch(ins){
            case 'f':
            fetch(client_socket);
            break;
            case 'v':
            view(client_socket);
            break;
            case 'h':
            intro();
            break;
            default:
            printf("Wrong!Input again or input \"h\"for Help\n");
            break;
        }
        closesocket(client_socket);
    } 
    
    WSACleanup();
    return 0;

}

void intro(){
    printf("This is a file tranferer\n");
    printf("Input \"f\" to Fetch file from server\n");
    printf("Input \"v\" to View the directory of the files\n");
    printf("Input \"e\" to Exit\n\n");
}


void fetch(SOCKET client_socket){
    
    char filename[100];
    
    printf("Enter the filename you want to download: ");
    fgets(filename, 100, stdin);
    filename[strcspn(filename, "\n")] = 0;

    char buffer_name[sizeof(Request)];
    MarshalRequest(buffer_name, 0, filename);
    send(client_socket, buffer_name, sizeof(Request), 0);

    // file create
    char sufix[]=".bak";
    strcat(filename,sufix);
    filename[strcspn(filename, "\n")] = 0;
    FILE *fp = fopen(filename, "wb");
    if (fp == NULL) {
        printf("File creation failed.\n");
        return;
    }

    // receive
    int bytes_received;
    char buffer[sizeof(Response)];
    unsigned int code;
    unsigned int size;
    char data[MAX_BUFFER_SIZE];

    //to show progress
    int file_size;
    int total_transferred = 0;
    int transfer_size;    

    do {
        bytes_received = recv(client_socket, buffer, sizeof(Response), 0);
        if (bytes_received <= 0) {
            printf("Connection closed by server.\n");
            fclose(fp);
            return;
        }
        UnmarshalResponse(buffer,&code, &size, data);
        if (code==START_TRANSFER)
            file_size = size;
        else if (code==TRANSFERING){
            fwrite(data, sizeof(char), size, fp);
            
        //progresss
            transfer_size = size;
            total_transferred += transfer_size;

            int progress = (int)((double)total_transferred / file_size * 100);

            updateProgress(progress);

            // Sleep(500);                    
        }

    } while (code == START_TRANSFER || code == TRANSFERING);

    fclose(fp);
    if (total_transferred==0 || code == FA1LED){
        printf("ERROR! No Bytes received ,but we still saved it as \"%s\"\n",filename);
        printf("Do you want to remove it? [y/N]:");
        char c;
        scanf(" %c",&c);
        fflush(stdin);
        if (c=='y')
            remove(filename);
        printf("\n");
    }
    else
        printf("\n%d Bytes received, and saved as %s\n\n",total_transferred,filename);
}

void view(SOCKET client_socket){
    printf("Directory of the files:\n");
    char buffer_view[sizeof(Request)];
    MarshalRequest(buffer_view, 1, NULL);


    send(client_socket, buffer_view, sizeof(Request), 0);

    int bytes_received;
    char buffer[sizeof(Response)];
    unsigned int code;
    unsigned int size;
    char data[MAX_BUFFER_SIZE];

    //receive
    do {
        bytes_received = recv(client_socket, buffer, sizeof(Response), 0);
        if (bytes_received <= 0) {
            printf("Connection closed by server.\n");
            return;
        }
        UnmarshalResponse(buffer,&code, &size, data);
        if(code == TRANSFERING) 
            printf("%s", data);
    }  while(code == START_TRANSFER || code == TRANSFERING);
    printf("\n\n");
}

void updateProgress(int progress) {
    int barWidth = 100;

    printf("[");
    int pos = barWidth * progress / 100;
    for (int i = 0; i < barWidth; ++i) {
        if (i < pos)
            printf("=");
        else if (i == pos)
            printf(">");
        else
            printf(" ");
    }
    printf("] %d%%\r", progress);
    fflush(stdout);
}