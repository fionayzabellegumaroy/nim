#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>

#define main not_main
#include "../nimd.c"
#undef main

int longName()
{
    printf("_________________________________________________\n\n");
    printf("Test One: Testing if program correctly handles long names.\n\n");

    client clients[1];

    int status = validate_name(clients, 1, 5, "haluhglauirhglauihrglauhgrlurhglsiuhrgauhrglaurhgauhglauhrglaurghaluirghaiughaiuhgr");

    fprintf(stdout, "Name length: %lu\n\n", strlen("haluhglauirhglauihrglauhgrlurhglsiuhrgauhrglaurhgauhglauhrglaurghaluirghaiughaiuhgr"));

    fprintf(stdout, "Status returned: %d\n\n", status);

    if (status)
    {
        printf("\tTest failed: test incorrectly did not catch the error.\n");
        return 1;
    }

    printf("Test suceeded: Name length exceeds 72 characters as expected.\n");
    return 0;
}

int alreadyPlaying()
{
    printf("_________________________________________________\n\n");
    printf("Test Two: Testing if program correctly handles already playing names.\n\n");

    client clients[2];
    int num_clients = 2;

    client player_one = {.socket_fd = 5, .name = "Alice", .state = PLAYING, .game_id = 0};

    clients[0] = player_one;

    fprintf(stdout, "Player One Name: %s\n\n", clients[0].name);
    fprintf(stdout, "Test Name: %s\n\n", "Alice");

    int status = validate_name(clients, num_clients, player_one.socket_fd, player_one.name);

    fprintf(stdout, "Status returned: %d\n\n", status);

    if (status)
    {
        printf("\tTest failed: test incorrectly did not catch the error.\n");
        return 1;
    }

    printf("Test suceeded: Name is already playing.\n");
    return 0;
}

int doubleOpen() // tbd
{
    printf("_________________________________________________\n\n");
    printf("Test Three: Testing if program correctly handles double open.\n\n");

    client clients[2];
    int num_clients = 2;

    client player_one = {.socket_fd = 5, .name = "Alice", .state = PLAYING, .game_id = 0};

    clients[0] = player_one;

    fprintf(stdout, "Player One Name: %s\n\n", clients[0].name);
    fprintf(stdout, "Test Name: %s\n\n", "Alice");

    int status = validate_name(clients, num_clients, player_one.socket_fd, player_one.name);

    fprintf(stdout, "Status returned: %d\n\n", status);

    if (status)
    {
        printf("\tTest failed: test incorrectly did not catch the error.\n");
        return 1;
    }

    printf("Test suceeded: Name is already playing and as expected, test printed an error.\n");
    return 0;
}

int moveBeforeWait() // tbd
{
    printf("_________________________________________________\n\n");
    printf("Test Four: Test if client sends MOVE before receiving WAIT.\n\n");

    // int status = validate_name(clients, num_clients, player_one.socket_fd, player_one.name);

    // fprintf(stdout, "Status returned: %d\n\n", status);

    // if (status)
    // {
    //     printf("\tTest failed: test incorrectly did not catch the error.\n");
    //     return 1;
    // }

    // printf("Test suceeded: Name is already playing and as expected, test printed an error.\n");
    return 0;
}

int moveBeforeName() // tbd
{
    printf("_________________________________________________\n\n");
    printf("Test Five: Test if client sends MOVE before receiving NAME.\n\n");

    // int status = validate_name(clients, num_clients, player_one.socket_fd, player_one.name);

    // fprintf(stdout, "Status returned: %d\n\n", status);

    // if (status)
    // {
    //     printf("\tTest failed: test incorrectly did not catch the error.\n");
    //     return 1;
    // }

    // printf("Test suceeded: Name is already playing and as expected, test printed an error.\n");
    return 0;
}

int moveNotTurn()
{
    printf("_________________________________________________\n\n");
    printf("Test Six: Tests program response to client sending MOVE when it is not their turn.\n\n");

    // int status = validate_name(clients, num_clients, player_one.socket_fd, player_one.name);

    // fprintf(stdout, "Status returned: %d\n\n", status);

    // if (status)
    // {
    //     printf("\tTest failed: test incorrectly did not catch the error.\n");
    //     return 1;
    // }

    // printf("Test suceeded: Name is already playing and as expected, test printed an error.\n");
    return 0;
}

int notAllowedMovePileIndex() // tbd
{
    printf("_________________________________________________\n\n");
    printf("Test Seven: Test if client sends MOVE with an incorrect pile index.\n\n");

    // int status = validate_name(clients, num_clients, player_one.socket_fd, player_one.name);

    // fprintf(stdout, "Status returned: %d\n\n", status);

    // if (status)
    // {
    //     printf("\tTest failed: test incorrectly did not catch the error.\n");
    //     return 1;
    // }

    // printf("Test suceeded: Name is already playing and as expected, test printed an error.\n");
    return 0;
}

int notAllowedMoveQuantity() // tbd
{
    printf("_________________________________________________\n\n");
    printf("Test Eight: Test if client sends MOVE with an incorrect quantity.\n\n");

    // int status = validate_name(clients, num_clients, player_one.socket_fd, player_one.name);

    // fprintf(stdout, "Status returned: %d\n\n", status);

    // if (status)
    // {
    //     printf("\tTest failed: test incorrectly did not catch the error.\n");
    //     return 1;
    // }

    // printf("Test suceeded: Name is already playing and as expected, test printed an error.\n");
    return 0;
}

int correctFormat()
{
    printf("_________________________________________________\n\n");
    printf("Test Nine: Pass the correct format to check_framing_errors.\n\n");

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

int lengthGreaterThan5()
{
    printf("_________________________________________________\n\n");
    printf("Test Ten: Tests whether length is greater than 5.\n\n");

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

int length2Digits()
{
    printf("_________________________________________________\n\n");
    printf("Test Eleven: Tests whether length is 2 decimal digits long.\n\n");

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

int messageShort()
{
    printf("_________________________________________________\n\n");
    printf("Test Twelve: Tests whether function can determine if content length is shorter than stated length.\n\n");

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

int messageLong()
{
    printf("_________________________________________________\n\n");
    printf("Test Thirteen: Tests whether function can determine if content length is longer than stated length.\n\n");

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

int lengthMoreThan104()
{
    printf("_________________________________________________\n\n");
    printf("Test Fourteen: Tests whether function can determine if entire message length is more than 104 characters.\n\n");

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

int incorrectVersion()
{
    printf("_________________________________________________\n\n");
    printf("Test Fifteen: Test validates that version is 0. \n\n");

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

int extraFields()
{
    // tbd
    return 0;
}

int emptyFields()
{
    // tbd
    return 0;
}

int sessionOne()
{
    printf("_________________________________________________\n\n");
    printf("Test Sixteen: Test if program correctly handles behavior of client disconnecting before being placed in a game.\n\n");

    // tbd
    return 0;
}

int sessionTwo()
{
    printf("_________________________________________________\n\n");
    printf("Test Seventeen: Test if program correctly handles behavior of client disconnecting during a game -> forfeit.\n\n");

    // tbd
    return 0;
}

int main(int argc, char *argv[])
{
    int failures = 0;

    // protocol errors
    failures += longName();
    failures += alreadyPlaying();
    failures += doubleOpen();
    failures += moveBeforeWait();
    failures += moveBeforeName();
    failures += moveNotTurn();
    failures += notAllowedMovePileIndex();
    failures += notAllowedMoveQuantity();

    // presentation errors
    failures += correctFormat();
    failures += lengthGreaterThan5();
    failures += length2Digits();
    failures += messageShort();
    failures += messageLong();
    failures += lengthMoreThan104();
    failures += incorrectVersion();

    // failures += extraFields();
    // failures += emptyFields();

    // session errors
    failures += sessionOne();
    failures += sessionTwo();

    printf("\n========================================\n");
    printf("Test Summary:\n");
    printf("  Passed: %d/%d\n", 17 - failures, 17);
    printf("========================================\n");

    // return number of failures (0 = all passed)
    return failures;
}