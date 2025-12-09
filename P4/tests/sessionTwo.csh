#!/bin/bash
# end to end testing for a complete game with no errors or forfeits

PORT=8086
pkill -f nimd 2>/dev/null
sleep 1

./nimd $PORT &
SERVER_PID=$!
sleep 3

echo "=== Testing Program Behavior After Client Disconnects During A Game ==="

# Player One Moves
{
    echo "0|11|OPEN|Jason|"   
    sleep 10
} | ./rawc localhost $PORT &

# Player Two Moves
{
    sleep 2
    echo "0|11|OPEN|Fiona|"
    sleep 15
} | ./rawc localhost $PORT &

sleep 10  #offset to allow game to finish

kill $SERVER_PID 2>/dev/null

echo "=== Test Completed ==="