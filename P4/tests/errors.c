#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>

#define main not_main
#include "../nimd.c"
#undef main

/* helper.c is linked in via the Makefile; do not include the .c file here to avoid duplicate symbols */

// int runInInteractive() // skeleton
// {
//     printf("_________________________________________________\n\n");
//     printf("Test One: Testing if program will run in interactive mode.\n\n");

//     char *argv[] = {"mysh"};
//     int argc = sizeof(argv) / sizeof(argv[0]);

//     int status = initializeShell(argc, argv);
//     (void) status;

//     printf("\nInteractive: %d\n\n", interactive);

//     if (!interactive)
//     {
//         printf("Test failed: Interactive mode test failed.\n");
//         return 1;
//     }

//     printf("Test suceeded: Interactive mode test succeeded.\n");
//     return 0;
// }

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

int correctFormat(){
    printf("_________________________________________________\n\n");
    printf("Test One: Pass the correct format to check_framing_errors.\n\n");

    int status = check_framing_errors("0|11|OPEN|ALICE|", 16);

    fprintf(stdout, "Message: %s\n", "0|11|OPEN|ALICE|");
    fprintf(stdout, "Status: %d\n\n", status);

    if (status != 0)
    {
        printf("Test failed: Expected status to be 0.\n");
        return 1;
    }
    printf("Test suceeded: Status is 0, meaning no framing errors detected.\n");
    return 0;
}

int lengthGreaterThan5(){
    printf("_________________________________________________\n\n");
    printf("Test Two: Tests whether length is greater than 5.\n\n");

    int status = check_framing_errors("0|03|...|", 9);

    fprintf(stdout, "Message: %s\n", "0|03|...|");
    fprintf(stdout, "Status: %d\n\n", status);

    if (status != -1)
    {
        printf("Test failed: Expected status to be -1.\n");
        return 1;
    }
    printf("Test suceeded: Status is -1 because length should be greater than 5.\n");
    return 0;
}

int length2Digits(){
    printf("_________________________________________________\n\n");
    printf("Test Three: Tests whether length is 2 decimal digits long.\n\n");

    int status = check_framing_errors("0|9|OPEN|BOB|", 13);

    fprintf(stdout, "Message: %s\n", "0|9|OPEN|BOB|");
    fprintf(stdout, "Status: %d\n\n", status);

    if (status != -1)
    {
        printf("Test failed: Expected status to be -1.\n");
        return 1;
    }
    printf("Test suceeded: Status is -1 because length should be two decimal digits long.\n");
    return 0;
}

int messageShort(){
    printf("_________________________________________________\n\n");
    printf("Test Four: Tests whether function can determine if content length is shorter than stated length.\n\n");

    int status = check_framing_errors("0|18|OPEN|ALICE|", 16);

    fprintf(stdout, "Message: %s\n", "0|18|OPEN|ALICE|");
    fprintf(stdout, "Status: %d\n\n", status);

    if (status != -1)
    {
        printf("Test failed: Expected status to be -1.\n");
        return 1;
    }
    printf("Test suceeded: Status is -1 because content length should match stated length.\n");
    return 0;
}

int messageLong(){
    printf("_________________________________________________\n\n");
    printf("Test Five: Tests whether function can determine if content length is longer than stated length.\n\n");

    int status = check_framing_errors("0|05|OPEN|ALICE|", 16);

    fprintf(stdout, "Message: %s\n", "0|05|OPEN|ALICE|");
    fprintf(stdout, "Status: %d\n\n", status);

    if (status != -1)
    {
        printf("Test failed: Expected status to be -1.\n");
        return 1;
    }
    printf("Test suceeded: Status is -1 because content length should match stated length.\n");
    return 0;
}

int lengthMoreThan104(){
    printf("_________________________________________________\n\n");
    printf("Test Six: Tests whether function can determine if entire message length is more than 104 characters.\n\n");

    int status = check_framing_errors("0|110|...|", 116);

    fprintf(stdout, "Message: %s\n", "0|110|...|");
    fprintf(stdout, "Status: %d\n\n", status);

    if (status != -1)
    {
        printf("Test failed: Expected status to be -1.\n");
        return 1;
    }
    printf("Test suceeded: Status is -1 because entire message length should not exceed 104 characters.\n");
    return 0;
}

int incorrectVersion(){
    printf("_________________________________________________\n\n");
    printf("Test Seven: Test validates that version is 0. \n\n");

    int status = check_framing_errors("1|11|OPEN|ALICE|", 16);

    fprintf(stdout, "Message: %s\n", "1|11|OPEN|ALICE|");
    fprintf(stdout, "Status: %d\n\n", status);

    if (status != -1)
    {
        printf("Test failed: Expected status to be -1.\n");
        return 1;
    }
    printf("Test suceeded: Status is -1 because version should be 0.\n");
    return 0;
}

int extraFields(){
    //tbd
    return 0;
}

int emptyFields(){
    //tbd
    return 0;
}



int main(int argc, char *argv[])
{
    int failures = 0;

    failures += correctFormat();
    failures +- lengthGreaterThan5();
    failures += length2Digits();
    failures += messageShort();
    failures += messageLong();
    failures += lengthMoreThan104();
    failures += incorrectVersion();

    // failures += extraFields();
    // failures += emptyFields();

    printf("\n========================================\n");
    printf("Test Summary:\n");
    printf("  Passed: %d/%d\n", 7 - failures, 7);
    printf("========================================\n");

    // return number of failures (0 = all passed)
    return failures;
}