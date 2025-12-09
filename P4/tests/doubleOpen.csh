#!/bin/bash

PORT=8087
pkill -f nimd 2>/dev/null
sleep 1

./nimd $PORT &
SERVER_PID=$!
sleep 3

echo "=== Testing Program Behavior After A Double Open ==="

# {
#     echo "0|11|OPEN|Jason|"   
#     sleep 7
#     echo "0|11|OPEN|Jason|"   

# } | ./rawc localhost $PORT 


{
    echo "0|11|OPEN|Jason|"   
    sleep 2
} | ./rawc localhost $PORT &


{
    echo "0|11|OPEN|Jason|"   
    sleep 7
} | ./rawc localhost $PORT 

sleep 10  #offset to allow game to finish

kill $SERVER_PID 2>/dev/null

echo "=== Test Completed ==="