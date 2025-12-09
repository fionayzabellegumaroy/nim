#!/bin/bash

PORT=8088
pkill -f nimd 2>/dev/null
sleep 1

./nimd $PORT &
SERVER_PID=$!
sleep 3

echo "=== Testing Program Behavior After Client Sends MOVE Message Before a NAME Message From Server ==="

{
    echo "0|11|OPEN|Jason|"   
    sleep 7
    echo "0|09|MOVE|3|5|"   

} | ./rawc localhost $PORT 

sleep 10  #offset to allow game to finish

kill $SERVER_PID 2>/dev/null

echo "=== Test Completed ==="