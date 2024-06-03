#include<stdio.h>
#include<stdlib.h>
#include<WinSock2.h>

#include"..\\api\\src\\ezTransferer.c"

#pragma comment(lib,"ws2_32.lib")

#define PORT 8080
#define BUFFER_SIZE 1024

void fetch(SOCKET);
void intro();
void view(SOCKET);


int main() {
    char ins;

    printf("///EZTRANSFER///\n");

    WSADATA wsa;
    SOCKET client_socket;
    struct sockaddr_in server;
    
    char server_ip[16];

    if (WSAStartup(MAKEWORD(2,2), &wsa) != 0) {
        printf("WSAStartup failed.\n");
        return 1;
    }

    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("Socket creation failed.\n");
        return 1;
    }

    printf("Enter the server IP address: ");
    fgets(server_ip, sizeof(server_ip), stdin);
    server_ip[strcspn(server_ip, "\n")] = 0;
    
    
    // server config
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(server_ip);
    server.sin_port = htons(PORT);

    if (connect(client_socket, (struct sockaddr *)&server, sizeof(server)) < 0) {
        printf("Connection failed.\n");
        return 1;
    }
    printf("Enter An Instruction:(\"h\"for Help)\n");
    scanf(" %c",&ins);
    fflush(stdin);
    while(ins != 'e'){
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
        printf("Enter An Instruction:(\"h\"for Help)\n");
        scanf(" %c",&ins);
        fflush(stdin);
    }

    closesocket(client_socket);
    WSACleanup();
    return 0;

}

void intro(){
    printf("This is a file tranferer\n");
    printf("Input \"f\" to Fetch file from server\n");
    printf("Input \"v\" to View the directory of the files\n");
    printf("Input \"e\" to Exit\n");
}


void fetch(SOCKET client_socket){
    
    char filename[100];
    
    printf("\nEnter the filename you want to download: ");
    fgets(filename, 100, stdin);
    // 去除换行符
    filename[strcspn(filename, "\n")] = 0;

    char buffer_name[sizeof(Request)];
    MarshalRequest(buffer_name, 0, filename);
    send(client_socket, buffer_name, sizeof(Request), 0);

    // 创建并打开文件以写入
    FILE *fp = fopen(filename, "wb");
    if (fp == NULL) {
        printf("File creation failed.\n");
    }

    // receive
    int bytes_received;
    char buffer[sizeof(Response)];
    unsigned int code;
    unsigned int size;
    char data[32517];

    while ((bytes_received = recv(client_socket, buffer, sizeof(Response), 0)) > 0) {
        
        UnmarshalResponse(buffer,&code, &size, data);
        if (code==TRANSFERING)
            fwrite(data, 1, size, fp);
    }

    fclose(fp);
    printf("File downloaded successfully.\n");

}

void view(SOCKET client_socket){
    char buffer_view[sizeof(Request)];
    MarshalRequest(buffer_view, 1, NULL);


    send(client_socket, buffer_view, sizeof(Request), 0);

    int bytes_received;
    char buffer[sizeof(Response)];
    unsigned int code;
    unsigned int size;
    char data[32517];
    
    //receive

    while ((bytes_received = recv(client_socket, buffer, sizeof(Response), 0)) > 0) {
        
        UnmarshalResponse(buffer,&code, &size, data);
        if(code==2)
            printf("%s",data);

    }
    printf("\n");
}

