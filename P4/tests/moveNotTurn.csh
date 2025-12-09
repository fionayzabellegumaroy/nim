#!/bin/bash
# end to end testing for a complete game with no errors or forfeits

PORT=8089
pkill -f nimd 2>/dev/null
sleep 1

./nimd $PORT &
SERVER_PID=$!
sleep 3

echo "=== Testing Program Behavior After Client Sends A Move When Not Their Turn ==="

# Player One Moves
{
    echo "0|11|OPEN|Jason|"   
    sleep 12
} | ./rawc localhost $PORT &

# Player Two Moves
{
    sleep 4
    echo "0|11|OPEN|Fiona|"
    sleep 4
    
    echo "0|09|MOVE|1|1|" 
    sleep 7
} | ./rawc localhost $PORT &

sleep 40  #offset to allow game to finish

kill $SERVER_PID 2>/dev/null

echo "=== Test Completed ==="