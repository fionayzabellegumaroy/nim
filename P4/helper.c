#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "nimd.h"
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
    else if (strcmp(type, "MOVE") == 0 && field_count == 1 && buf[check_length - 1] == '|')
    {
        return 1;
    }

    // fprintf(stdout, "return 0\n"); // debug print
    return 0;
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
    write(socket_fd, msg, sizeof(msg));
    return;
}

// will this call ever fail? if so, change to int return type
void send_fail_msg(int socket_fd, char *reason)
{
    char msg[256];
    int msg_len = 6 + strlen(reason);
    snprintf(msg, sizeof(msg), "0|%02d|FAIL|%s|", msg_len, reason);
    write(socket_fd, msg, strlen(msg));
    return;
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

// helper function to just check if a player is available
int hasAvailablePlayer(client clients[], int num_clients)
{
    for (int i = 0; i < num_clients; i++)
    {
        if (clients[i].state == WAITING)
        {
            return 1; // Found one
        }
    }
    return 0; // None found
}

// helper function to count waiting players
int countWaitingPlayers(client clients[], int num_clients)
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

void close_client_sockets(client clients[], int num_clients)
{
    for (int i = 0; i < num_clients; i++)
    {
        close(clients[i].socket_fd);
    }
}