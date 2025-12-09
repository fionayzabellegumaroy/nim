#!/bin/bash

PORT=8080
pkill -f nimd 2>/dev/null
sleep 1

./nimd $PORT &
SERVER_PID=$!
sleep 3

echo "=== Testing MOVE Message ==="

# Player One Moves
{
    echo "0|11|OPEN|Jason|"   
    sleep 7
    
    echo "0|09|MOVE|0|1|"    
} | ./rawc localhost $PORT &

# Player Two Moves
{
    sleep 4
    echo "0|11|OPEN|Fiona|"
    sleep 7
    
    echo "0|09|MOVE|1|1|" 
} | ./rawc localhost $PORT &

sleep 10  #offset to allow game to finish

kill $SERVER_PID 2>/dev/null

echo "=== Test Completed ==="