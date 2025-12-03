#include "clients/src/network.h"
#include "clients/src/pbuf.h"

typedef struct {
    int version; // always 0
    int length; // length of message after length and its terminator (|)
    char* type;
} message ;

typedef enum {
    CONNECTED,
    WAITING,
    PLAYING,
    DISCONNECTED
} client_state_t;

typedef struct {
    char player1_name[73];  //72 plus its null terminator
    char player2_name[73];  //72 plus its null terminator
    int board[5];           // for simplicity, num. of stones in each pile will be 1, 3, 5, 7, and 9 stones respectively
    int current_player;     // 1 or 2
    int player1_socket;     
    int player2_socket;
} game_instance;

typedef struct {
    int socket_fd;          
    char name[73];        
    client_state_t state;  
    int game_id; // this will probably only matter really when we have multiple games but still a good idea to have rn
} client;

// from helper.c
void send_fail_msg(int socket_fd, char* reason);
int send_play_msg(int socket_fd, int pile, int stones);
int send_wait_msg(int socket_fd);
int validate_fields(char buf[], int check_length, char* type);


// might need to remove this before submission

// from clients/pbuf.h
// void print_buffer(char *, unsigned int); // not currently in use but maybe we will need it or something

// from clients/network.h
int connect_inet(char *host, char *service);
int open_listener(char *service, int queue_size);