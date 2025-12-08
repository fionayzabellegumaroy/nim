#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "nimd.h"

void close_client_sockets(client clients[], int num_clients)
{
    for (int i = 0; i < num_clients; i++)
    {
        close(clients[i].socket_fd);
    }
}

// helper function to count waiting players
int num_waiting_players(client clients[], int num_clients)
{
    int count = 0;
    for (int i = 0; i < num_clients; i++)
    {
        if (clients[i].state == WAITING)
        {
            count++;
        }
    }

    printf("waiting players: %d\n", count); // debug print
    return count;
}

void print_parsed_msg(char *msg[], int count)
{
    fprintf(stdout, "print parsed msg:\n");
    if (msg == NULL || count <= 0)
        return;
    for (int i = 0; i < count; i++)
    {
        if (msg[i] == NULL)
            break;
        fprintf(stdout, "%s\n", msg[i]);
    }
    return;
}

// will this call ever fail? if so, change to int return type
void send_fail_msg(int socket_fd, char *reason)
{
    char msg[256];
    int msg_len = 6 + strlen(reason);
    snprintf(msg, sizeof(msg), "0|%02d|FAIL|%s|", msg_len, reason);
    write(socket_fd, msg, strlen(msg));
    write(socket_fd, "\n", 1);

    return;
}

// temp
int send_play_msg(int socket_fd, int pile, int stones)
{
    return 0;
}

void send_wait_msg(int socket_fd)
{
    char msg[] = "0|05|WAIT|";
    printf("Sending wait message: %s\n", msg);

    size_t len = strlen(msg);
    write(socket_fd, msg, len);   
    write(socket_fd, "\n", 1);

    return;
}

int send_over_msg(int socket_fd, int winner, int board[5], const char *reason) {
    char msg[256];

    char board_str[64];
    snprintf(board_str, sizeof(board_str),
             "%d %d %d %d %d",
             board[0], board[1], board[2], board[3], board[4]);

    char body[256];
    snprintf(body, sizeof(body), "OVER|%d|%s|%s|", winner, board_str, reason ? reason : "");

    int body_len = (int)strlen(body);
    snprintf(msg, sizeof(msg), "0|%02d|%s", body_len, body);

    fprintf(stdout, "Sending OVER message: %s\n", msg);

    size_t len = strlen(msg);
    write(socket_fd, msg, len);   
    write(socket_fd, "\n", 1);   

    return (int)len;
}

// helper function to check if message is valid and is terminated within stated length
int validate_fields(char buf[], int check_length, char *type)
{
    int field_count = 0;
    int pos = 0;         // start after "OPEN|"
    int field_start = 0; // track start of current field

    while (pos < check_length)
    {
        // fprintf(stdout, "%c\n", buf[pos]); // debug print
        if (buf[pos] == '|')
        {
            // only count if there's actual content in this field
            if (pos > field_start)
            {
                field_count++;
            }
            field_start = pos + 1; // update -> next field starts after this |
        }
        pos++;
    }

    // fprintf(stdout, "field count: %d, type: %s, terminates %c\n", field_count, type, buf[check_length-1]); // debug print

    // check expected field count and proper termination
    if ((strcmp(type, "OPEN") == 0 || strcmp(type, "FAIL") == 0) && field_count == 2 && buf[check_length - 1] == '|')
    {
        return 1;
    }
    else if (strcmp(type, "MOVE") == 0 && field_count == 3 && buf[check_length - 1] == '|')
    {
        return 1;
    }

    // fprintf(stdout, "return 0\n"); // debug print
    return 0;
}

int validate_name(client clients[], int num_clients, int client_socket, char *name) // Change parameter name
{
    // checks for long name
    if (strlen(name) >= 73)
    {
        send_fail_msg(client_socket, "21 Long Name");
        return 0;
    }

    // checks for duplicate name
    for (int i = 0; i < num_clients; i++)
    {
        if (clients[i].state != CONNECTED && strcmp(clients[i].name, name) == 0)
        {
            send_fail_msg(client_socket, "22 Already Playing");
            return 0;
        }
    }
    
    return 1;
}