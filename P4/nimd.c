#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <netdb.h>
#include <signal.h>
#include <errno.h>
#include "nimd.h"

game_instance games[MAX_GAMES];
client clients[MAX_CLIENTS];

int main(int argc, char **argv){

    if (argc < 2){
        // deal with errors error handler later on
        fprintf(stderr, "Error: Not enough arguments\n"); // placeholder if there is an alternative, correct way
        exit(1);
    }

    int listener_socket = open_listener(argv[1], 10);

    if (listener_socket < 0){
        fprintf(stderr, "Error: Failed to create a listening socket\n");
    }

    fprintf(stdout, "nimd server listening on port %s\n", argv[1]);

    // server is running
    while(1){
        int client_socket = accept(listener_socket, NULL, NULL);

        if (client_socket < 0){
            fprintf(stderr, "Error: Client unable to connect to server\n");
        } else {
            fprintf(stdout, "Client %d successfully connected to server\n", client_socket);
        }

        write(client_socket, "You have successfully connected to nimd server", 47); // might not need this tbh

        // future: need to handle messages and whatnot!

        // close all sockets
        close(client_socket);
    }

    close(listener_socket);

    return 0;
}