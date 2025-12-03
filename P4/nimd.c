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

// might need to do realloc later if we have more clients/games than expected
#define MAX_CLIENTS 10 // arbitrary for now
#define MAX_GAMES 5    // arbitrary for now

game_instance games[MAX_GAMES];
client clients[MAX_CLIENTS];

char buf[256]; // max msg should be 104; but will allocate more just in case of incorrect msg formatting
char *modified[104];

int num_clients = 0;
int num_games = 0;

// function to check if message has any framing errors -> returns 0 if none; -1 if an error is found
int check_framing_errors(char buf[], int received_length)
{
    // check max message length
    if (received_length > 104) return -1;

    // 1. check version
    if (buf[0] != '0' || buf[1] != '|')
    {
        // fprintf(stderr, "version is not 0\n"); // debug print
        return -1; // version not 0 and no |
    }

    // 2. check length field format
    if (buf[2] < '0' || buf[2] > '9' || buf[3] < '0' || buf[3] > '9' || buf[4] != '|')
    {
        // fprintf(stderr, "length not two decimal digits\n"); // debug print
        return -1; // length not two decimal digits + |
    }

    // 3. validate length
    int stated_length = (buf[2] - '0') * 10 + (buf[3] - '0'); // converts ASCII digit chars into into ints
    if (stated_length < 5)
    {
        // fprintf(stderr, "length is not greater than 4\n"); // debug print
        return -1; // length is too short
    }

    // 4. check if type is four chars long and a valid [ client] type
    char type[5];
    strncpy(type, &buf[5], 4);
    type[4] = '\0';

    // will count all types since 3.4.2 just states it should match types from 3.2
    if (buf[9] != '|' || (strcmp(type, "OPEN") != 0 &&
                          strcmp(type, "WAIT") != 0 &&
                          strcmp(type, "NAME") != 0 &&
                          strcmp(type, "PLAY") != 0 &&
                          strcmp(type, "MOVE") != 0 &&
                          strcmp(type, "OVER") != 0 &&
                          strcmp(type, "FAIL") != 0))
    {
        return -1;
    }

    // 5. calculate content length (after msg length and '|' so we use num 5)
    int content_length = received_length - 5;
    fprintf(stdout, "received length: %d, content length: %d, stated length: %d\n", received_length, content_length, stated_length);

    // 6. check length mismatch and field structure
    if (content_length < stated_length && validate_fields(&buf[5], content_length, type))
    { // too short if all field are present and terminates
        fprintf(stdout, "msg too short\n");
        return -1;
    }
    else if (content_length >= stated_length && !validate_fields(&buf[5], stated_length, type))
    { // too long if not all fields are present and does not terminate within stated length
        fprintf(stdout, "msg too long\n");
        return -1;
    }

    return 0;
}

int main(int argc, char **argv)
{
    setvbuf(stdout, NULL, _IONBF, 0); // disable buffering for consistent prints

    if (argc < 2)
    {
        fprintf(stderr, "Error: Not enough arguments\n");
        exit(1);
    }

    int listener_socket = open_listener(argv[1], 10);

    if (listener_socket < 0)
    {
        fprintf(stderr, "Error: Failed to create listening socket\n");
        exit(1);
    }

    fprintf(stdout, "nimd server listening on port %s\n", argv[1]);

    while (1)
    {
        int client_socket = accept(listener_socket, NULL, NULL);

        if (client_socket < 0)
        {
            perror("accept");
            continue;
        }

        // on successful connection, create client struct
        client new_client;
        new_client.socket_fd = client_socket;
        new_client.state = CONNECTED;

        clients[num_clients] = new_client;
        num_clients++;

        fprintf(stdout, "Client %d connected\n", client_socket); // server-side log

        write(client_socket, "You have successfully connected to nimd server", 47); // client-side log

        // read message from client [ should be an OPEN message ]
        int bytes = read(client_socket, buf, 255);
        buf[bytes] = '\0';

        if (bytes <= 0)
        {
            fprintf(stderr, "Client disconnected\n");
            close(client_socket);
            continue;
        }

        // check for framing errors before parsing
        if (check_framing_errors(buf, strlen(buf)) != 0)
        {
            send_fail_msg(client_socket, "10 Invalid");
            close(client_socket);
        }

        // fprintf(stdout, "no framing errors for msg %s\n", buf); // debug print

        // if no framing errors, parse message
        char temp[256];
        if (parse_msg(strcpy(temp, buf)) != 0){

        }

        // after a succeessful OPEN from client, server sends WAIT message
        // int wait_status = send_wait_msg(client_socket);

        // check if there are 2 available players to start a game

        close(client_socket);
    }

    close(listener_socket);
    return 0;
}