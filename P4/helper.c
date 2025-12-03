#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

// helper function to check if message is valid and is terminated within stated length
int validate_fields(char buf[], int check_length, char* type) {
    int field_count = 0;
    int pos = 0; // start after "OPEN|"
    int field_start = 0;// track start of current field
    
    while (pos < check_length) {
        fprintf(stdout, "%c\n", buf[pos]);
        if (buf[pos] == '|') {
            // only count if there's actual content in this field
            if (pos > field_start) {
                field_count++;
            }
            field_start = pos + 1; // update -> next field starts after this |
        }
        pos++;
    }

    // fprintf(stdout, "field count: %d, type: %s, terminates %c\n", field_count, type, buf[check_length-1]); // debug print
    
    // check expected field count and proper termination 
    if ((strcmp(type, "OPEN") == 0 || strcmp(type, "FAIL") == 0) && field_count == 2 && buf[check_length -1] == '|') {
        fprintf(stdout, "return 1\n");
        return 1;
    } else if (strcmp(type, "MOVE") == 0 && field_count == 1 && buf[check_length -1] == '|') {
        return 1;
    }
    
    // fprintf(stdout, "return 0\n"); // debug print
    return 0;
}

// helper function to get 2 available players; returns 0
int availablePlayers(char clients[], char *names)
{
    return 0;
}

// temp
int send_play_msg(int socket_fd, int pile, int stones)
{
    return 0;
}

int send_wait_msg(int socket_fd)
{
    char msg[] = "0|05|WAIT";
    printf("Sending wait message: %s\n", msg);
    write(socket_fd, msg, sizeof(msg));
    return 0;
}

// will this call ever fail? if so, change to int return type
void send_fail_msg(int socket_fd, char *reason)
{
    char msg[256];
    snprintf(msg, sizeof(msg), "0|%02d|FAIL|%s", (int)(7 + strlen(reason)), reason);
    write(socket_fd, msg, strlen(msg));
    return;
}