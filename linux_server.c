#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#include <netinet/in.h>

#define TRUE   1 
#define FALSE  0 
#define PORT 8001
#define ALLOW_CONNETIONS 11 // 100 sockets 

static int master_socket, clients[ALLOW_CONNETIONS];

void respond(int slot){
    for (int i; i<10; i++){
        printf("Client: %d, ", clients[i]);
    }
    int valread;
    char *success_msg = "HTTP/1.1 200 OK\nContent-Type: text/html; charset=utf-8\n<h1>Hello</h1>";
    printf("Slot Number (response): %d \n", slot);

    //? reading response
    char buffer[1024] = {0};
    
    valread = read( clients[slot] , buffer, 1024);
    printf("%s\n",buffer );

    //? splitting buffer output
    char *method = strtok(buffer,  " \t\r\n");
    char *url = strtok(NULL, " \t");
    char *prot = strtok(NULL, " \t\r\n");
        
    printf("%s | %s | %s \n", method, url, prot);
    send(clients[slot] , success_msg , strlen(success_msg) , 0 );
        
        
    printf("Success message sent\n");

    // if (shutdown(clients[slot], SHUT_WR) < 0){
    //     perror("shutdown");
    //     exit(EXIT_FAILURE);
    // }

//    free(buffer);
    shutdown(STDOUT_FILENO, SHUT_WR);
    close(clients[slot]);
    clients[slot] = -1;

}


int main(int argc, char const *argv[]) {
    struct sockaddr_in address;
    int slot=0, opt=1;
    int addrlen = sizeof(address);

    // ?setting all elements to -1 means connection
    for (int i = 0; i < ALLOW_CONNETIONS+1; i++){
        clients[i]=-1;
    }
    

    //? Creating socket file descriptor
    if ((master_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    //? Forcefully attaching socket to the $PORT
    if (setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );

    //? Forcefully attaching socket to the $PORT
    if (bind(master_socket, (struct sockaddr *)&address, sizeof(address))<0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(master_socket, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    //?avoid zombie connections
    signal(SIGCHLD,SIG_IGN);

    //?accepting connections
    while (1){
        if ((clients[slot] = accept(master_socket, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        
        printf("Slot Number (main): %d \n", slot);
        if ( fork()==0 ) {
            //? I am now the client - close the listener: client doesnt need it
            close(master_socket);
            respond(slot);
            exit(0);
        } else {
            //? I am still the server - close the accepted handle: server doesnt need it.
            close(clients[slot]);
            
        }

        //while (clients[slot]!=-1) slot = (slot+1)%ALLOW_CONNETIONS;
        // TODO: after reaching 100 reset all slots, but check if someone is still there
        if (slot > 9){
            slot = 0;
        } else if (clients[slot+1] == -1) {
            slot++;
        }
        
    }

    return 0;
}
