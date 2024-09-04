#include <iostream>
#include <unistd.h>
#include <bits/stdc++.h>

#include <arpa/inet.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <pthread.h>
#include <time.h>

#define BUFLEN 2048
#define SERVER_IP_ADDRESS "127.0.0.1"
#define PORT 5300

struct client{
    int uid;
    int client_sock_fd;
};

std::vector<int> sock_fds;

void writeDatatoAllClients(const char *data){
    for(auto sock_fd : sock_fds){
        write(sock_fd, data, strlen(data));
    }
}

void *handleClient(void *client){
    struct client *c = (struct client*)client;
    int sock_fd = c->client_sock_fd;
    int uid = c->uid;

    std::cout << "User: " << uid << " joined!\n" << std::endl;
    sock_fds.push_back(sock_fd);
    char r_buff[BUFLEN];

    while(1){
        // Recieve data from a single client
        bzero(r_buff, sizeof(r_buff));
        int n = read(sock_fd, r_buff, sizeof(r_buff));
        if(n<=0){
            std::cout << "User " << uid << " disconnected!" << std::endl;
            break;
        }
    
        std::cout << "Client "<< uid <<  " says: " << r_buff << std::endl;

        // Write data to all clients
        writeDatatoAllClients(r_buff);
    }

    sock_fds.erase(std::remove(sock_fds.begin(), sock_fds.end(), sock_fd), sock_fds.end());
    close(sock_fd);
    free(c);
    return NULL;
}


int main(int argc, char **argv){

    int sock_fd, client_fd;
    int yes = 1;

    srand(time(NULL));

    // Internet socket address
    struct sockaddr_in server, client;
    socklen_t client_len;

    // Create a socket
    if((sock_fd = socket(AF_INET, SOCK_STREAM, 0))==-1){
        std::cout << "Failed to create a socket!" << std::endl;
        exit(1);
    }

    // Define server port, address and family
    server.sin_addr.s_addr = inet_addr(SERVER_IP_ADDRESS);
    server.sin_port = PORT;
    server.sin_family = AF_INET;

    // Set socket options
    if(setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes))==-1){
        std::cout << "Failed to set socket options" << std::endl;
        exit(1);
    }

    // Bind socket to server
    if(bind(sock_fd, (struct sockaddr *)&server, sizeof(server))==-1){
        std::cout << "Failed to bind socket to server" << std::endl;
        exit(1);
    }

    // Listen for connections
    listen(sock_fd, 5);
    std::cout << "Server started on port: " << PORT << "\n" << std::endl;

    while(1){
        // Accept any client connections
        client_len = sizeof(client);
        if((client_fd = accept(sock_fd, (struct sockaddr*)&client, &client_len))==-1){
            std::cout << "Failed to accept client!" << std::endl;
            exit(1);
        }

        // Assign unique random ID to every client and initialize the structure
        int uid = rand()%100;
        struct client *c = (struct client *)malloc(sizeof(struct client));
        c->client_sock_fd = client_fd;
        c->uid = uid;

        // Create a new thread for every client
        pthread_t tid;
        if(pthread_create(&tid, NULL, handleClient, (void *)c)!=0){
            std::cout << "Failed to create client thread" << std::endl;
        }

        // After the thread terminates, storage for tid can be reclaimed
        pthread_detach(tid);
    }

    close(sock_fd);

    return 0;
}