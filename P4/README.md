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

### Design:
1. Set up server that has one listening socket
2. Each client gets their own socket FD (which will be trackd with client_info)
3. Once a game starts, will utilize helper functions to send our messages and parse through the messages (i.e. int send_play_msg(int socket, int turn, int[] board) and int parse_move_msg(char *buffer, char *name) [ able to do char *name since names are unique, or we could also keep track giving a client struct idk])
4. and so on? 

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



## Questions For Project:
1. Do we delete clients folder before submission?
    If so, need to delete all references of it from nimd.h, Makefile, and maybe a few more