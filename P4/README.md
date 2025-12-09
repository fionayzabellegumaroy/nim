# nim

name: Jason Cruz Sipiran
netID: jc3003

name: Fiona Yzabelle Gumaroy
netID: fag46

## Instructions:
### MakeFile:

### Other:
To change the max games and players of the server, just change it in nimd.c, under MAX_CLIENTS and MAX_GAMES.

___

## System Design:

### Data structures:
1. struct message 
    - Defines the fields used in an NGP message (version, length, type).
2. enum client_state 
    - Tracks whether a client is:
        - CONNECTED
        - WAITING
        - PLAYING
        - DISCONNECTED
    - Lets the server quickly find players available to match.
3. struct client
    - Stores the client's socket FD, name, current state, and game_id.
    - game_id = -1 indicates the client is not currently in a game.
    - All clients are tracked in a global clients[] array with num_clients.
4. struct game_instance 
    - Stores both player names, both sockets, and the current board state.
    - Board represented as an array of 5 integers initialized to [1,3,5,7,9]. 
    - Stores which player's turn it is and the assigned game_id.
    - Games are tracked in games[] with num_games.
5. Buffer
    - A shared buf is used for receiving raw message bytes.
    - Token arrays hold pointers after splitting the input by |.

### Design:
1. Server begins by creating a single listening socket using open_listener.
2. Each new client connection receives its own socket FD, stored in a client struct.
3. Server expects the first valid message to be an OPEN request.
    - If invalid, server sends FAIL and closes the socket.
4. Name validation ensures the player name is unique and within length limits.
5. Valid clients are placed in WAITING state and sent a WAIT message.
6. When at least two clients are waiting, the server:
    - Selects the first two WAITING clients.
    - Creates a new game_instance, initializes board, assigns game_id.
    - Updates both clients’ states to PLAYING.
7. A new child process is forked to run the game:
    - Child handles all gameplay and communication for that match.
    - Parent removes those clients from the waiting list and continues accepting new clients.
8. Child sends initial NAME messages to both players, identifying player numbers.
9. All helper operations (send WAIT, FAIL, NAME, OVER, parsing, board logic) are delegated to functions in helper.c and pbuf.

### Game Flow:
1. Child process begins the match by sending:
    - NAME message to both players containing their player number and opponent name.
    - Initial PLAY message containing player turn + board state.
2. Game loop (run_game) uses select to monitor both player sockets.
    - Only the current player is allowed to send MOVE.
    - Wrong-turn moves result in FAIL|31 Impatient|.
    - Malformed messages result in FAIL|10 Invalid| and game ends by forfeit.
    - Disconnects result in immediate forfeit, with an OVER message sent to opponent.
3. For a valid MOVE:
    - Server checks pile index; invalid -> FAIL|32 Pile Index|.
    - Server checks quantity; invalid -> FAIL|33 Quantity|.
    - Legal moves update the board and continue the game.
4. If move empties the board:
    - Current player is winner -> send OVER to both clients with final board.
   Otherwise:
    - Turn switches to the other player.
    - Server sends updated PLAY message to both players.
5. Game loop continues until:
    - A player wins by taking the last stones
    - A player disconnects / violates protocol, causing a forfeit.

### Other:
1. We interpreted double open error to occur only after server processes another player connects; not in the same client terminal (if that makes sense).

## Tests:
__Important to Note: Sometimes even adding a buffer [sleep] to the script will cause issues, but when it doesn't, it works correctly.
### Logic Testing
__Requirement:__
This will test game flows for end to end testing.

__Detection method:__
This test program will be utilizing a script to run an end to end test. If the script prints out "Socket EOF or error" at the end of it and there are no errors messaged out, it is a success. If not, it means that there is an error (successfully printing out the error the way we want).

__Test:__
__Important to Note: If listener socket cannot be binded to, go to script itself and change the port #, signified by "PORT="__

1. correctFormat.csh: This script is an end to end test that is a successful game between two players (Fiona and Jason). The inputs are in the script "correctFormat.csh" [to avoid overclumping the readme, view it there if needed]. The test program should end with a "Socket EOF or error" since there are no more inputs, which signifies a success!

2. multiplayer(): This script is an end to end test that has three games that will start to test the multiplayer connection aspect of the program [3 will act as our mini stress test :)]. The inputs are in the "multiplayer.csh" [to avoid overclumping the readme, view it there if needed]. The test program should end with a "Socket EOF or error" since there are no more inputs, which signifies a success! Note: It will not specifically test full games with their own specific movesets since it that involves a more complicated test, and we already tested that the program runs a full game successfully. 
___

__Requirement:__
find_and_start_game() starts a game for two players given there are 2 WAITING players in the server [ or not if there are not at least 2 ]

__Detection method:__
This is a unit test: Given a clients array, amongst other parameters, to pipe through find_and_start_game(), the function returns a unique game id on success or -1 on failure. 

__Test:__
1. noWaiting(): clients[] will have two CONNECTED players to test function's capability to determine if a game can be started or not. This function should return -1 since there are no WAITING players. 

2. twoWaiting(): clients[] will have two WAITING players to test function's capability to determine if a game can be started or not. This function should return a game id (1 since there are 0 games before this function runs).

3. multipleWaiting(): clients[] will have 8 WAITING players to test function's capability to start multiple games [idk if this is necessarily needed; might be tested under the multiple players requirement] and test if function will give game IDs sequentially.

4. mixedStates() clients[] will have a variety of states: 3 WAITING, 2 CONNECTED, and 2 PLAYING. This test will test function's capability of only setting up a game given there is a variety of states in clients[].

___

### Protocol
__Requirement:__ 
Program correctly responds to all message types.

__Detection method:__ 
Test program utilizes a script that includes the following moves: OPEN message, MOVE message, FAIL message, and an unknown [not a type that is specified in the writeup] message. There will be statements printed out so we can detect if program correctly responds to all messages.

__Test:__
1. testOpen(): The script will have an input that just includes "0|11|OPEN|JASON" to test how program deciphers an OPEN messsage. The program should send a WAIT message to the client. 

2. testMove(): The script will have an input that just includes an OPEN message and "0|09|MOVE|3|5|" to test how program deciphers an MOVE messsage. The program should send updated PLAY messages to the clients. 

3. testFail(): The script will have an input that just includes a FAIL message and "0|24|FAIL|22 Already Playing|" to test how program deciphers an MOVE messsage. The program should just quietly exit.

4. testUnknown(): The script will have an input that that just includes "0|09|SLAY|3|5|" to test how program deciphers an unknown messsage. The program should send a FAIL message to the client. 

___

### Errors Conditions
__Requirement:__ 
Program correctly detects any session errors.

__Detection method:__ 
Test programs will utilize a script and the messages sent to client will reflect whether the program correctly fails given a protocol error, or not. 

__Test:__
1. sessionOne(): Test if program correctly handles behavior of client disconnecting before being placed in a game. It should print out an exiting.

2. sessionTwo() Test if program correctly handles behavior of client disconnecting during a game -> forfeit. It should print out a forfeit message, leading the other player to win.
___

__Requirement:__ 
Program correctly detects any protocol errors.

__Detection method:__ 
Tests will utilize printing out the status to reflect whether the program correctly fails given a protocol error, or not. Or, it will run a script, and the messages sent to client will reflect whether the program correctly fails given a protocol error, or not. 


__Test:__
1. longName(): Test if program correctly handles behavior of client sending OPEN with a name longer than 72 chars (server should respond with FAIL and message code 21 Long Name and close the connection). To show this, test program will return use validate_name that will return 0 if there's an error and will return 1 if there's no error.

2. alreadyPlaying():  Test if program correctly handles behavior of client sending OPEN with a name already used by a player in an on-going game (server should respond with FAIL and message code 22 Already Playing and close the connection). To show this, test program will return use validate_name that will return 0 if there's an error and will return 1 if there's no error.

3. doubleOpen.csh:  Test if program correctly handles behavior of client sending OPEN a second time (server should respond with FAIL and message code 23 Already Open and close the connection). 

4. moveBeforeWait.csh:  Test if program correctly handles behavior of client sending MOVE before game begin before receiving WAIT (server should respond with FAIL and message code 24 Not Playing and close the connection). 

5. moveNotTurn.csh:  Test if program correctly handles behavior of client sending MOVE when it is not their turn (server should respond with FAIL and message code 31 Impatient and close the connection). 

6. notAllowedMovePileIndex.csh:  Test if program correctly handles behavior of client sending a move that is not allowed, specifically incorrect pile index, (server should respond with FAIL and message code 32 Pile Index and close the connection).

7. notAllowedMoveQuantity.csh:  Test if program correctly handles behavior of client sending a move that is not allowed, specifically incorrect quantity, (server should respond with FAIL and message code 33 Quantity and close the connection). 
___

__Requirement:__ 
check_framing_errors() ensures that the message is correctly formatted as stated in the writeup.

__Detection method:__ 
This is a unit test: given an array of parameters to pipe through check_framing_errors, the function will return -1 if there is a presentation error and 0 if there is none. 

__Test:__
1. correctFormat(): Input will be "0|11|OPEN|ALICE|" to act as a positive test for the function. Function should return 0, meaning function correctly checks.

2. lengthGreaterThan5(): Input will be "0|03|...|" to test if function is capable of testing whether a length is less than 5. This should return -1, meaning function correctly checks error. 

3. length2Digits(): Input will be "0|9|OPEN|BOB|" to test if function is capable of testing whether the message length is two decimal digits followed by a vertical bar. This should return -1, meaning function correctly checks error.

4. messageShort(): Input will be "0|18|OPEN|ALICE|" to test if function is capable of determining whether the stated message length matches content length [ messagee after "0|message length|" ]. This should return -1, meaning function correctly determines that the content length is shorter than the stated length.

5. messageLong(): Input will be "0|05|OPEN|ALICE|" to test if function is capable of determining whether the stated message length matches content length [ messagee after "0|message length|" ]. This should return -1, meaning function correctly determines that the content length is longer than the stated length.

6. lengthMoreThan104(): Input will be "0|110|...|" to test if function is capable of determining whether entire message length is less than 105 bytes.
 
7. incorrectVersion(): Input will be "1|11|OPEN|ALICE|" to test if function correctly determines if message only has version 0. Test will return -1 since messages should only have 0 for version. 
___