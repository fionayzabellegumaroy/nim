# nim

name: Jason Cruz Sipiran
netID: jc3003

name: Fiona Yzabelle Gumaroy
netID: fag46

## System Design:

### Data structures:
1. struct message defines the required field needed in a NGP message (version, length, and type).
2. enum client_state defines whether a client is connected, waiting, playing, or disconnected
3. struct game_instance defines the names of both player one and player two, the sockets of both players, the board state using an int array, and the current player. 
4. struct client defines the socket file descriptor, name, state, and the game id (when/if we choose to implement multiple games systems)
    a. game id = -1 if not in a game yet

### Design:
1. Set up server that has one listening socket
2. Each client gets their own socket FD (which will be tracked with client struct)
3. Once there are 2 available clients in clients[], match them to a game
4. Once a game starts, will utilize helper functions to send our messages and parse through the messages (i.e. int send_play_msg(int socket, int turn, int[] board) and int parse_move_msg(char *buffer, char *name) [ able to do char *name since names are unique, or we could also keep track giving a client struct idk])
5. and so on? 

thoughts: if we decide to work on multiple games, to decide who to match, we just grab the earliest waiting clients

### Game Flow:
1. Client1 connects and sends: 0|16|OPEN|alice| (assuming "alice" is the name)
2. Server responds to client1: 0|05|WAIT|
3. Client2 connects and sends: 0|14|OPEN|bob|
4. Server responds to client2: 0|05|WAIT|

__Game Start:__ 
Server sends to client1: 0|17|NAME|1|bob| 
Server sends to client2: 0|19|NAME|2|alice|

__Game Flow:__
Server sends both clients the same PLAY message: 0|17|PLAY|1|1 3 5 7 9| 
Player 1 should respond with MOVE
Server updates board and sends new PLAY to both clients with updated state and next player number
Player 2 should respond with MOVE
Process repeats

## Tests:
### Logic Testing
__Requirement:__
This will test one game flow to see if everything actually works
__Detection method:__

__Test:__
1. correctFormat():
2. protocolError():
3. sessionError():
4. framingErrors():
5. multiplayer():

___

__Requirement:__
find_and_start_game() starts a game for two players given there are 2 WAITING players in the server [ or not if there are not at least 2 ]

__Detection method:__
Given a clients array, amongst other parameters, to pipe through find_and_start_game(), the function returns a unique game id on success or -1 on failure. 

__Test:__
1. noWaiting(): clients[] will have two CONNECTED players to test function's capability to determine if a game can be started or not. This function should return -1 since there are no WAITING players. 

2. twoWaiting(): clients[] will have two WAITING players to test function's capability to determine if a game can be started or not. This function should return a game id (1 since there are 0 games before this function runs).

3. multipleWaiting(): clients[] will have 8 WAITING players to test function's capability to start multiple games [idk if this is necessarily needed; might be tested under the multiple players requirement] and test if function will give game IDs sequentially.

4. mixedStates() clients[] will have a variety of states: 3 WAITING, 2 CONNECTED, and 2 PLAYING. This test will test function's capability of only setting up a game given there is a variety of states in clients[].

### Protocol
__Requirement:__ 
Program correctly responds to all message types.

__Detection method:__ 
tbd

__Test:__
1. testOpen():

2. testMove():

3. testFail():

4. testUnknown():

___

__Requirement:__  [idk about this one]
There is a correct message flow. tbd [might not need to test this tbh]

__Detection method:__ 
tbd but maybe we put into an array the clients' message flow and sees if correct flow, and same could be for server hmm

__Test:__

### Errors Conditions
__Requirement:__ 
Program correctly detects any session errors.

__Detection method:__ 
Tests will utilize printing out the status to reflect whether the program correctly fails given a protocol error, or not. 

__Test:__
1. sessionOne(): Test if program correctly handles behavior of client disconnecting before being placed in a game.

2. sessionTwo() Test if program correctly handles behavior of client disconnecting during a game -> forfeit. 

__Requirement:__ 
Program correctly detects any protocol errors.

__Detection method:__ 
Tests will utilize printing out the status to reflect whether the program correctly fails given a protocol error, or not. 

__Test:__
1. longName(): Test if program correctly handles behavior of client sending OPEN with a name longer than 72 chars (server should respond with FAIL and message code 21 Long Name and close the connection). To show this, test program will return use validate_name that will return 0 if there's an error and will return 1 if there's no error.

2. alreadyPlaying():  Test if program correctly handles behavior of client sending OPEN with a name already used by a player in an on-going game (server should respond with FAIL and message code 22 Already Playing and close the connection). To show this, test program will return use validate_name that will return 0 if there's an error and will return 1 if there's no error.

3. doubleOpen():  Test if program correctly handles behavior of client sending OPEN a second time (server should respond with FAIL and message code 23 Already Open and close the connection). To show this, test program will initiate a FAIL message with correct code and print out the state of disconnected player [IF WE DECIDE TO KEEP DISCONNECTED STATE]

4. moveBeforeWait():  Test if program correctly handles behavior of client sending MOVE before game begin before receiving WAIT (server should respond with FAIL and message code 24 Not Playing and close the connection). To show this, test program will initiate a FAIL message with correct code and print out the state of disconnected player [IF WE DECIDE TO KEEP DISCONNECTED STATE]

5. moveBeforeName():  Test if program correctly handles behavior of client sending MOVE before game begin before receiving NAME (server should respond with FAIL and message code 24 Not Playing and close the connection). To show this, test program will initiate a FAIL message with correct code and print out the state of disconnected player [IF WE DECIDE TO KEEP DISCONNECTED STATE]

6. moveNotTurn():  Test if program correctly handles behavior of client sending MOVE when it is not their turn (server should respond with FAIL and message code 31 Impatient and close the connection). To show this, test program will initiate a FAIL message with correct code and print out the state of disconnected player [IF WE DECIDE TO KEEP DISCONNECTED STATE]

7. notAllowedMovePileIndex():  Test if program correctly handles behavior of client sending a move that is not allowed, specifically incorrect pile index, (server should respond with FAIL and message code 32 Pile Index and close the connection). To show this, test program will initiate a FAIL message with correct code and print out the state of disconnected player [IF WE DECIDE TO KEEP DISCONNECTED STATE]

8. notAllowedMoveQuantity():  Test if program correctly handles behavior of client sending a move that is not allowed, specifically incorrect quantity, (server should respond with FAIL and message code 33 Quantity and close the connection). To show this, test program will initiate a FAIL message with correct code and print out the state of disconnected player [IF WE DECIDE TO KEEP DISCONNECTED STATE]


__Requirement:__ 
check_framing_errors() ensures that the message is correctly formatted as stated in the writeup.

__Detection method:__ 
Given an array of parameters to pipe through check_framing_errors, the function will return -1 if there is a presentation error and 0 if there is none. 

__Test:__
1. correctFormat(): Input will be "0|11|OPEN|ALICE|" to act as a positive test for the function. Function should return 0, meaning function correctly checks.

2. lengthGreaterThan5(): Input will be "0|03|...|" to test if function is capable of testing whether a length is less than 5. This should return -1, meaning function correctly checks error. 

3. length2Digits(): Input will be "0|9|OPEN|BOB|" to test if function is capable of testing whether the message length is two decimal digits followed by a vertical bar. This should return -1, meaning function correctly checks error.

4. messageShort(): Input will be "0|18|OPEN|ALICE|" to test if function is capable of determining whether the stated message length matches content length [ messagee after "0|message length|" ]. This should return -1, meaning function correctly determines that the content length is shorter than the stated length.

5. messageLong(): Input will be "0|05|OPEN|ALICE|" to test if function is capable of determining whether the stated message length matches content length [ messagee after "0|message length|" ]. This should return -1, meaning function correctly determines that the content length is longer than the stated length.

6. lengthMoreThan104(): Input will be "0|110|...|" to test if function is capable of determining whether entire message length is less than 105 bytes.
 
7. incorrectVersion(): Input will be "1|11|OPEN|ALICE|" to test if function correctly determines if message only has version 0. Test will return -1 since messages should only have 0 for version. 

8. extraFields():

9. emptyFields(): 

### Other
1. argv is not 2
2. cannot open listening socket

__Requirement:__ 

__Detection method:__ 

__Test:__

## Questions For Project:
1. Do we delete clients folder before submission?
    If so, need to delete all references of it from nimd.h, Makefile, and maybe a few more
2. If due to a presentation error that leads to closing a connection, does that mean it technically counts as a forfeit?
3. If player sends OPEN more than once but during a play, we close both connections?
4. Writeup says that client can send FAIL, but doesn't specify how server should handle it. We can maybe close the socket id OR we log that fail and then continue on.
5. Where do we want to check if version is 0? I currently have it under framing errors
6. Do we want to treat client giving server type messages as a framing error (rn i put it as is)
7. How do we want to deal with empty fields like "0|15|OPEN|ALICE||" and extra fields [ im thinking we can deem this as an error ]
8. Spec says,"client sending OPEN with a name already used by a player in an on-going game" is an error, but doesn't this imply that if a player is WAITING or CONNECTED we can use that name? I kind of don't like that, but idk if that is a design choice we can make.   
