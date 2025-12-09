#!/bin/bash

PORT=8085
pkill -f nimd 2>/dev/null
sleep 1

./nimd $PORT &
SERVER_PID=$!
sleep 3

echo "=== Testing unknown Message ==="

# Player One Moves
{
    echo "0|11|SLAY|Jason|"   
    sleep 7
} | ./rawc localhost $PORT
sleep 10  #offset to allow game to finish

kill $SERVER_PID 2>/dev/null

echo "=== Test Completed ==="