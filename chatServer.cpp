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

// Structure to manage client info
struct client{
    char username[20];
    int uid;
    int client_sock_fd;
};

// Keep track of FDs of all connected clients
std::vector<int> sock_fds;

// Mutex for sync
pthread_mutex_t sock_fds_mutex PTHREAD_MUTEX_INITIALIZER;

// Broadcasts received messages to all clients
void broadcastMsg(const char *data, const char *username){
    char message[BUFLEN];
    snprintf(message, sizeof(message), "%s : %s", username, data);

    pthread_mutex_lock(&sock_fds_mutex);
    for(auto sock_fd : sock_fds){
        write(sock_fd, message, strlen(message));
    }
    pthread_mutex_unlock(&sock_fds_mutex);
}

// Read data from clients
void *handleClient(void *client){
    char username[20];
    bzero(username, sizeof(username));
    struct client *c = (struct client*)client;
    int sock_fd = c->client_sock_fd;
    int uid = c->uid;
    strcpy(username, c->username);
    username[strcspn(username, "\n")] = '\0';

    std::cout << username << " joined!\n" << std::endl;
    pthread_mutex_lock(&sock_fds_mutex);
    sock_fds.push_back(sock_fd);
    pthread_mutex_unlock(&sock_fds_mutex);
    char r_buff[BUFLEN];

    while(1){
        // Recieve data from a single client
        bzero(r_buff, sizeof(r_buff));
        int n = read(sock_fd, r_buff, sizeof(r_buff));
        if(n<=0){
            std::cout << username << " disconnected!" << std::endl;
            break;
        }
    
        std::cout << username <<  " says: " << r_buff << std::endl;

        // Broadcast data to all clients
        broadcastMsg(r_buff, username);
    }

    pthread_mutex_lock(&sock_fds_mutex);
    sock_fds.erase(std::remove(sock_fds.begin(), sock_fds.end(), sock_fd), sock_fds.end());
    pthread_mutex_unlock(&sock_fds_mutex);
    close(sock_fd);
    free(c);
    return NULL;
}


int main(int argc, char **argv){

    int sock_fd, client_fd;
    int yes = 1;
    char username[20];

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

        // Read user details from client
        bzero(username, sizeof(username));
        if(read(client_fd, username, sizeof(username))==-1){
            std::cout << "Failed to read client details!" << std::endl;
        }
        int uid = rand()%100;
        struct client *c = (struct client *)malloc(sizeof(struct client));
        c->client_sock_fd = client_fd;
        c->uid = uid;
        strcpy(c->username, username);

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