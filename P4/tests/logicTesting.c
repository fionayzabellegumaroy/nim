#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>

#define main not_main
#include "../nimd.c"
#undef main

int noWaiting()
{
    printf("_________________________________________________\n\n");
    printf("Test One: No waiting players.\n\n");

    client clients[2];
    int num_clients = 2;
    game_instance games[2];
    int num_games = 0;

    client player_one = {.socket_fd = 1, .name = "Alice", .state = CONNECTED, .game_id = -1};
    client player_two = {.socket_fd = 2, .name = "Bob", .state = CONNECTED, .game_id = -1};

    clients[0] = player_one;
    clients[1] = player_two;

    int game_id = find_and_start_game(clients, num_clients, games, &num_games);

    fprintf(stdout, "Player %s state: %s\n", clients[0].name, clients[0].state ? "OTHER" : "CONNECTED");
    fprintf(stdout, "Player %s state: %s\n\n", clients[1].name, clients[1].state ? "OTHER" : "CONNECTED");
    fprintf(stdout, "Game id: %d\n\n", game_id);

    if (game_id != -1)
    {
        printf("Test failed: Expected game id to be -1.\n");
        return 1;
    }

    printf("Test suceeded: Game id is -1, since no players are waiting.\n");
    return 0;
}

int twoWaiting()
{
    printf("_________________________________________________\n\n");
    printf("Test Two: Two waiting players.\n\n");

    client clients[2];
    int num_clients = 2;
    game_instance games[2];
    int num_games = 0;

    client player_one = {.socket_fd = 1, .name = "Alice", .state = WAITING, .game_id = -1};
    client player_two = {.socket_fd = 2, .name = "Bob", .state = WAITING, .game_id = -1};

    clients[0] = player_one;
    clients[1] = player_two;

    int game_id = find_and_start_game(clients, num_clients, games, &num_games);

    fprintf(stdout, "Player %s state: %s game id: %d\n", clients[0].name, clients[0].state ? "OTHER" : "CONNECTED", clients[0].game_id);
    fprintf(stdout, "Player %s state: %s game id: %d\n\n", clients[1].name, clients[1].state ? "OTHER" : "CONNECTED", clients[1].game_id);
    fprintf(stdout, "Game id: %d\n\n", game_id);

    if (game_id != 0)
    {
        printf("Test failed: Expected game id to be 0.\n");
        return 1;
    }

    printf("Test suceeded: Game id is 0, since two players are waiting.\n");
    return 0;
}

int mixedStates()
{
    printf("_________________________________________________\n\n");
    printf("Test Three: Mixed player states.\n\n");

    client clients[6];
    int num_clients = 6;
    game_instance games[3];
    int num_games = 1;

    client player_one = {.socket_fd = 1, .name = "Alice", .state = WAITING, .game_id = -1};
    client player_two = {.socket_fd = 2, .name = "Bob", .state = CONNECTED, .game_id = -1};
    client player_three = {.socket_fd = 1, .name = "Cat", .state = PLAYING, .game_id = 0};
    client player_four = {.socket_fd = 2, .name = "Dylan", .state = WAITING, .game_id = -1};
    client player_five = {.socket_fd = 1, .name = "Ethan", .state = PLAYING, .game_id = 0};
    client player_six = {.socket_fd = 2, .name = "Fiyero", .state = CONNECTED, .game_id = -1};

    clients[0] = player_one;
    clients[1] = player_two;
    clients[2] = player_three;
    clients[3] = player_four;
    clients[4] = player_five;
    clients[5] = player_six;

    game_instance existing_game = {
        .game_id = 0,
        .player1_socket = 1,
        .player2_socket = 2,
        .current_turn = 1};
    games[0] = existing_game;

    fprintf(stdout, "Before finding and starting game: \n");
    fprintf(stdout, "\tPlayer %s state: %d game id: %d\n", clients[0].name, clients[0].state, clients[0].game_id);
    fprintf(stdout, "\tPlayer %s state: %d game id: %d\n", clients[1].name, clients[1].state, clients[1].game_id);
    fprintf(stdout, "\tPlayer %s state: %d game id: %d\n", clients[2].name, clients[2].state, clients[2].game_id);
    fprintf(stdout, "\tPlayer %s state: %d game id: %d\n", clients[3].name, clients[3].state, clients[3].game_id);
    fprintf(stdout, "\tPlayer %s state: %d game id: %d\n", clients[4].name, clients[4].state, clients[4].game_id);
    fprintf(stdout, "\tPlayer %s state: %d game id: %d\n\n", clients[5].name, clients[5].state, clients[5].game_id);

    int game_id = find_and_start_game(clients, num_clients, games, &num_games);

    fprintf(stdout, "After finding and starting game: \n");
    fprintf(stdout, "\tPlayer %s state: %d game id: %d\n", clients[0].name, clients[0].state, clients[0].game_id);
    fprintf(stdout, "\tPlayer %s state: %d game id: %d\n", clients[1].name, clients[1].state, clients[1].game_id);
    fprintf(stdout, "\tPlayer %s state: %d game id: %d\n", clients[2].name, clients[2].state, clients[2].game_id);
    fprintf(stdout, "\tPlayer %s state: %d game id: %d\n", clients[3].name, clients[3].state, clients[3].game_id);
    fprintf(stdout, "\tPlayer %s state: %d game id: %d\n", clients[4].name, clients[4].state, clients[4].game_id);
    fprintf(stdout, "\tPlayer %s state: %d game id: %d\n", clients[5].name, clients[5].state, clients[5].game_id);

    fprintf(stdout, "Game id: %d\n\n", game_id);

    if (game_id != 1)
    {
        printf("Test failed: Expected game id to be 1.\n");
        return 1;
    }

    printf("Test suceeded: Game id is 1, since one game is already happening -> next id available is 1.\n");
    return 0;
}

int main(int argc, char *argv[])
{
    int failures = 0;

    failures += noWaiting();
    failures += twoWaiting();
    failures += mixedStates();

    printf("\n========================================\n");
    printf("Test Summary:\n");
    printf("  Passed: %d/%d\n", 3 - failures, 3);
    printf("========================================\n");

    return failures;
}