#include <iostream>
#include <unistd.h>
#include <iomanip>
#include <bits/stdc++.h>

#include <arpa/inet.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <pthread.h>

#define BUFLEN 2048
#define SERVER_IP_ADDRESS "127.0.0.1"
#define PORT 5300

void *readMessagesFromServer(void *sock_fd_arg){
    int *sock_fd = (int *)sock_fd_arg;
    char r_buff[BUFLEN];

    while(1){
        sleep(2);
        // Read messages from server and display
        bzero(r_buff, sizeof(r_buff));
        read(*sock_fd, r_buff, sizeof(r_buff));
        std::cout << "\n----------------------------------------" << std::endl;
        std::cout << "\n\nServer messages: \n\n" << std::endl;
        std::cout << r_buff << std::endl;
        std::cout << "----------------------------------------" << std::endl;
    }
}

void chat(int sock_fd){
    char s_buff[BUFLEN];

    while(1){
        // Send data to server
        bzero(s_buff, sizeof(s_buff));
        std::cout << "Enter a message or command to send: ";
        std::cout << "\n*****\n";
        std::cout << "Available commands: \n" << std::setw(20) << std::endl;
        std::cout << "/whisper  - " << "Send data to a particular client based on their ID  - [Example usage: /whisper 29 Hello James]" << std::setw(10);
        std::cout << "\n*****\n";

        fgets(s_buff, sizeof(s_buff), stdin);
        write(sock_fd, s_buff, strlen(s_buff));
    }
}

int main(int agrc, char **argv){

    int sock_fd;
    struct sockaddr_in server;
    pthread_t tid;

    // Get username
    char username[20];
    std::cout << "Enter your name: ";
    fgets(username, sizeof(username), stdin);

    // Create a socket
    if((sock_fd = socket(AF_INET, SOCK_STREAM, 0))==-1){
        std::cout << "Failed to create a socket!" << std::endl;
        exit(1);
    }

    // Server details
    server.sin_addr.s_addr = inet_addr(SERVER_IP_ADDRESS);    // server IP address
    server.sin_family = AF_INET;
    server.sin_port = PORT;

    // Connect to server
    if(connect(sock_fd, (struct sockaddr*)&server, sizeof(server))==-1){
        std::cout << "Failed to connet to server!" << std::endl;
        exit(1);
    }

    // Send user details to sever
    if(write(sock_fd, username, strlen(username))==-1){
        std::cout << "Failed to register!" << std::endl;
        exit(1);
    }

    // Thread to read messages from server
    pthread_create(&tid, NULL, readMessagesFromServer, &sock_fd);

    // Interact with other connected clients
    chat(sock_fd);

    // Close fd on disconnecting
    close(sock_fd);

    return 0;
}