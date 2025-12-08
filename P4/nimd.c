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
#include <sys/select.h>
#include <sys/time.h>
#include "nimd.h"

// might need to do realloc later if we have more clients/games than expected
#define MAX_CLIENTS 10 // arbitrary for now
#define MAX_GAMES 10   // arbitrary for now

game_instance games[MAX_GAMES];
client clients[MAX_CLIENTS];

char buf[256]; // max msg should be 104; but will allocate more just in case of incorrect msg formatting
char *modified[104];

int num_clients = 0;
int num_games = 0;

static int board_all_zero(int board[5]) {
    for (int i = 0; i < 5; i++) {
        if (board[i] != 0) return 0;
    }
    return 1;
}

// function to check if message has any framing errors -> returns 0 if none; -1 if an error is found
int check_framing_errors(char buf[], int received_length)
{
    if (received_length > 104) // check max message length
        return -1;

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
    // fprintf(stdout, "received length: %d, content length: %d, stated length: %d\n", received_length, content_length, stated_length); // debug print

    // 6. check length mismatch and field structure
    if (content_length < stated_length && validate_fields(&buf[5], content_length, type))
    { // too short if all field are present and terminates
        return -1;
    }
    else if (content_length >= stated_length && !validate_fields(&buf[5], stated_length, type))
    { // too long if not all fields are present and does not terminate within stated length
        return -1;
    }

    return 0;
}

// function to parse through message
void parse_msg(char buf[], char *msg[], char token_storage[][256], int max_tokens)
{
    char *token;
    int count = 0;
    token = strtok(buf, "|");

    while (token != NULL && count < max_tokens)
    {
        strcpy(token_storage[count], token);
        msg[count] = token_storage[count];

        count++;
        token = strtok(NULL, "|");
    }

    // print_parsed_msg(msg, count); // debug
    return;
}

int send_name_msg(int socket_fd, int player_num, const char *opponent_name) {
    char msg[256];

    char body[256];
    snprintf(body, sizeof(body), "NAME|%d|%s|", player_num, opponent_name);

    int body_len = (int)strlen(body);         
    snprintf(msg, sizeof(msg), "0|%02d|%s", body_len, body);

    fprintf(stdout, "Sending NAME message: %s\n", msg);

    size_t len = strlen(msg);
    write(socket_fd, msg, len);   
    write(socket_fd, "\n", 1); 

    return (int)len;
}

int send_play_state(int socket_fd, int next_player, int board[5]) {
    char msg[256];

    char board_str[64];
    snprintf(board_str, sizeof(board_str),
             "%d %d %d %d %d",
             board[0], board[1], board[2], board[3], board[4]);

    char body[256];
    snprintf(body, sizeof(body), "PLAY|%d|%s|", next_player, board_str);

    int body_len = (int)strlen(body);
    snprintf(msg, sizeof(msg), "0|%02d|%s", body_len, body);

    fprintf(stdout, "Sending PLAY message: %s\n", msg); 

    size_t len = strlen(msg);
    write(socket_fd, msg, len); 
    write(socket_fd, "\n", 1);

    return (int)len;
}

// function to find and start a game -> on success, return game id; on failure, return -1
int find_and_start_game(client clients[], int num_clients, game_instance games[], int *num_games)
{
    int player1_index = -1;
    int player2_index = -1;
    int found = 0;

    // find two players in WAITING state
    for (int i = 0; i < num_clients && found < 2; i++)
    {
        if (clients[i].state == WAITING)
        {
            if (found == 0)
            {
                player1_index = i;
                found++;
            }
            else
            {
                player2_index = i;
                found++;
            }
        }
    }

    // if two players waiting, start a game
    if (found == 2)
    {
        // create new game instance
        game_instance new_game;
        new_game.game_id = *num_games; // ID = game #
        new_game.player1_socket = clients[player1_index].socket_fd;
        new_game.player2_socket = clients[player2_index].socket_fd;
        strcpy(new_game.player1_name, clients[player1_index].name);
        strcpy(new_game.player2_name, clients[player2_index].name);
        new_game.current_turn = 1; // player 1 starts

        // initialize board
        new_game.board[0] = 1;
        new_game.board[1] = 3;
        new_game.board[2] = 5;
        new_game.board[3] = 7;
        new_game.board[4] = 9;

        // add game to games array
        games[*num_games] = new_game;
        (*num_games)++;

        // update client states
        clients[player1_index].state = PLAYING;
        clients[player1_index].game_id = new_game.game_id;
        clients[player2_index].state = PLAYING;
        clients[player2_index].game_id = new_game.game_id;

        // printf("Started game %d: %s vs %s\n",
        //        new_game.game_id, new_game.player1_name, new_game.player2_name); // debug print

        // add play msg here!!!!!!!!!!!
        return new_game.game_id; // return game id
    }

    return -1; // no game started
}

void run_game(game_instance *g)
{
    int current_player = g->current_turn;

    while (1) {
        int p1 = g->player1_socket;
        int p2 = g->player2_socket;
        int cur_sock   = (current_player == 1) ? p1 : p2;
        int other_sock = (current_player == 1) ? p2 : p1;

        send_play_state(p1, current_player, g->board);
        send_play_state(p2, current_player, g->board);

        while (1) {
            fd_set readfds;
            FD_ZERO(&readfds);
            FD_SET(cur_sock, &readfds);
            FD_SET(other_sock, &readfds);
            int maxfd = (p1 > p2 ? p1 : p2) + 1;

            int ready = select(maxfd, &readfds, NULL, NULL, NULL);
            if (ready < 0) {
                perror("select");
                send_over_msg(p1, 0, g->board, "Forfeit");
                send_over_msg(p2, 0, g->board, "Forfeit");
                close(p1);
                close(p2);
                return;
            }

            if (FD_ISSET(other_sock, &readfds)) {
                int bytes = read(other_sock, buf, sizeof(buf) - 1);
                if (bytes <= 0) {
                    send_over_msg(cur_sock,
                                  current_player,
                                  g->board,
                                  "Forfeit");
                    close(cur_sock);
                    close(other_sock);
                    return;
                }
                buf[bytes] = '\0';

                if (check_framing_errors(buf, bytes) != 0) {
                    send_fail_msg(other_sock, "10 Invalid");
                    send_over_msg(cur_sock,
                                  current_player,
                                  g->board,
                                  "Forfeit");
                    close(cur_sock);
                    close(other_sock);
                    return;
                }

                char temp[256];
                char *msg[6];
                char token_storage[10][256];
                strcpy(temp, buf);
                parse_msg(temp, msg, token_storage, 6);

                if (strcmp(msg[2], "MOVE") == 0) {
                    send_fail_msg(other_sock, "31 Impatient");
                    continue;
                } else {
                    send_fail_msg(other_sock, "10 Invalid");
                    send_over_msg(cur_sock,
                                  current_player,
                                  g->board,
                                  "Forfeit");
                    close(cur_sock);
                    close(other_sock);
                    return;
                }
            }

            if (FD_ISSET(cur_sock, &readfds)) {
                int bytes = read(cur_sock, buf, sizeof(buf) - 1);
                if (bytes <= 0) {

                    fprintf(stderr, "Player %d disconnected, forfeit\n", current_player);
                    send_over_msg(other_sock,
                                  (current_player == 1) ? 2 : 1,
                                  g->board,
                                  "Forfeit");
                    close(cur_sock);
                    close(other_sock);
                    return;
                }
                buf[bytes] = '\0';

                if (check_framing_errors(buf, bytes) != 0) {
                    send_fail_msg(cur_sock, "10 Invalid");
                    send_over_msg(other_sock,
                                  (current_player == 1) ? 2 : 1,
                                  g->board,
                                  "Forfeit");
                    close(cur_sock);
                    close(other_sock);
                    return;
                }

                char temp[256];
                char *msg[6];
                char token_storage[10][256];
                strcpy(temp, buf);
                parse_msg(temp, msg, token_storage, 6);

                if (strcmp(msg[2], "MOVE") != 0) {
                    if (strcmp(msg[2], "OPEN") == 0) {
                        send_fail_msg(cur_sock, "23 Already Open");
                    } else {
                        send_fail_msg(cur_sock, "10 Invalid");
                    }
                    send_over_msg(other_sock,
                                  (current_player == 1) ? 2 : 1,
                                  g->board,
                                  "Forfeit");
                    close(cur_sock);
                    close(other_sock);
                    return;
                }

                int pile = atoi(msg[3]);  
                int qty  = atoi(msg[4]);

                if (pile < 0 || pile >= 5) {
                    send_fail_msg(cur_sock, "32 Pile Index");
                    continue;   
                }

                if (qty <= 0 || qty > g->board[pile]) {
                    send_fail_msg(cur_sock, "33 Quantity");
                    continue;  
                }

                g->board[pile] -= qty;

                if (board_all_zero(g->board)) {
                    int winner = current_player;
                    send_over_msg(g->player1_socket, winner, g->board, "");
                    send_over_msg(g->player2_socket, winner, g->board, "");
                    close(g->player1_socket);
                    close(g->player2_socket);
                    return;
                }

                current_player = (current_player == 1) ? 2 : 1;
                break;
            }

        }
    } 
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
        int status;
        while (waitpid(-1, &status, WNOHANG) > 0) { }

        int client_socket = accept(listener_socket, NULL, NULL);
        if (client_socket < 0)
        {
            perror("accept");
            continue;
        }

        fprintf(stdout, "Client %d connected\n", client_socket);

        int bytes = read(client_socket, buf, 255);
        if (bytes <= 0) {
            fprintf(stderr, "Client disconnected before sending OPEN\n");
            close(client_socket);
            continue;
        }
        buf[bytes] = '\0';

        if (check_framing_errors(buf, bytes) != 0) {
            send_fail_msg(client_socket, "10 Invalid");

            close(client_socket);
            continue;
        }

        char temp[256];
        char *msg[5];
        char token_storage[10][256];

        strcpy(temp, buf);
        parse_msg(temp, msg, token_storage, 5);

        if (strcmp(msg[2], "OPEN") != 0) 
        {
            if (strcmp(msg[2], "MOVE") == 0) {
                send_fail_msg(client_socket, "24 Not Playing");
            } else {
                send_fail_msg(client_socket, "10 Invalid");
            }

            close(client_socket);
            continue;
        }


        if (!validate_name(clients, num_clients, client_socket, msg[3])) 
        {
            close(client_socket);
            continue;
        }

        client new_client;
        new_client.socket_fd = client_socket;
        new_client.state = WAITING;
        new_client.game_id = -1;

        strcpy(new_client.name, msg[3]);

        send_wait_msg(client_socket);

        clients[num_clients] = new_client;
        num_clients++;

        if (num_waiting_players(clients, num_clients) >= 2) 
        {
            int game_id = find_and_start_game(clients, num_clients, games, &num_games);
            
            if (game_id != -1) 
            {
                game_instance *g = &games[game_id];

                pid_t pid = fork();
                if (pid < 0) {
                    perror("fork");

                    send_fail_msg(g->player1_socket, "10 Invalid");
                    send_fail_msg(g->player2_socket, "10 Invalid");
                    
                    close(g->player1_socket);
                    close(g->player2_socket);
                } 
                else if (pid == 0) {
                    close(listener_socket);

                    send_name_msg(g->player1_socket, 1, g->player2_name);
                    send_name_msg(g->player2_socket, 2, g->player1_name);

                    run_game(g);

                    exit(0);
                } 
                else {
                    close(g->player1_socket);
                    close(g->player2_socket);

                    for (int i = 0; i < num_clients; i++) 
                    {
                        if (clients[i].game_id == g->game_id) 
                        {
                            clients[i] = clients[num_clients - 1];

                            num_clients--;
                            i--;
                        }
                    }
                }
            }
        }
    }

    close_client_sockets(clients, num_clients);
    close(listener_socket);
    return 0;
}