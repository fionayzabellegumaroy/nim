#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>

#define main not_main
#include "../nimd.c"
#undef main

int testOpen() // tbd
{
    printf("_________________________________________________\n\n");
    printf("Test One: Tests program's response after receiving an OPEN message.\n\n");

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

int testMove() // tbd
{
    printf("_________________________________________________\n\n");
    printf("Test Two: Tests program's response after receiving a MOVE message.\n\n");

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

int testFail() // tbd
{
    printf("_________________________________________________\n\n");
    printf("Test Three: Tests program's response after receiving a FAIL message.\n\n");

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

int testUnknown() // tbd
{
    printf("_________________________________________________\n\n");
    printf("Test Four: Tests program's response after receiving a message that is of unknown type.\n\n");

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

int main(int argc, char *argv[])
{
    int failures = 0;

   failures += testOpen();
   failures += testMove();
   failures += testFail();
   failures += testUnknown();

    printf("\n========================================\n");
    printf("Test Summary:\n");
    printf("  Passed: %d/%d\n", 4 - failures, 4);
    printf("========================================\n");

    // return number of failures (0 = all passed)
    return failures;
}