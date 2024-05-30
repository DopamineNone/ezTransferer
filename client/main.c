#include<stdio.h>
#include<stdlib.h>
#include<WinSock2.h>

#include"..\\api\\src\\ezTransferer.c"

#pragma comment(lib,"ws2_32.lib")

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    WSADATA wsa;
    SOCKET client_socket;
    struct sockaddr_in server;
    
    char filename[100];
    char server_ip[16];

    // initialize Winsock
    if (WSAStartup(MAKEWORD(2,2), &wsa) != 0) {
        printf("WSAStartup failed.\n");
        return 1;
    }

    // client socket
    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("Socket creation failed.\n");
        return 1;
    }

    printf("Enter the server IP address: ");
    fgets(server_ip, sizeof(server_ip), stdin);
    server_ip[strcspn(server_ip, "\n")] = 0;
    
    
    // server config
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(server_ip); // 服务器IP地址
    server.sin_port = htons(PORT);

    // 连接到服务器
    if (connect(client_socket, (struct sockaddr *)&server, sizeof(server)) < 0) {
        printf("Connection failed.\n");
        return 1;
    }


    // 输入要下载的文件名
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
        return 1;
    }

    // receive
    int bytes_received;
    char buffer[sizeof(Response)];
    unsigned int code;
    unsigned int size;
    char data;

    while ((bytes_received = recv(client_socket, buffer, sizeof(Response), 0)) > 0) {
        
        UnmarshalResponse(buffer,&code, &size, &data);
        if (code==TRANSFERING)
            fwrite(data, 1, size, fp);
        else
            continue;
    }

    fclose(fp);
    printf("File downloaded successfully.\n");

    closesocket(client_socket);
    WSACleanup();
    return 0;
}
