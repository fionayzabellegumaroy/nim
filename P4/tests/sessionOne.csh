#!/bin/bash

PORT=8086
pkill -f nimd 2>/dev/null
sleep 1

./nimd $PORT &
SERVER_PID=$!
sleep 3

echo "=== Testing Program Behavior After Client Disconnects Before Being Placed In A Game ==="

{
    echo "0|11|OPEN|Jason|"   
    sleep 7
    
} | ./rawc localhost $PORT 

sleep 10  #offset to allow game to finish

kill $SERVER_PID 2>/dev/null

echo "=== Test Completed ==="