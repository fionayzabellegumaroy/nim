#!/bin/bash
# end to end testing for a complete game with no errors or forfeits

PORT=8080
pkill -f nimd 2>/dev/null
sleep 1

./nimd $PORT &
SERVER_PID=$!
sleep 3

echo "=== Testing Successful Game W/ No Errors ==="

# Player One Moves
{
    echo "0|11|OPEN|Jason|"   
    sleep 7
    
    echo "0|09|MOVE|0|1|"    
    sleep 7
    echo "0|09|MOVE|1|2|"  
    sleep 7
    echo "0|09|MOVE|2|3|"    
    sleep 7
    echo "0|09|MOVE|3|4|"   
    sleep 7
    echo "0|09|MOVE|4|5|"   
    sleep 7
} | ./rawc localhost $PORT &

# Player Two Moves
{
    sleep 4
    echo "0|11|OPEN|Fiona|"
    sleep 7
    
    echo "0|09|MOVE|1|1|" 
    sleep 7
    echo "0|09|MOVE|2|2|"    
    sleep 7
    echo "0|09|MOVE|3|3|"   
    sleep 7
    echo "0|09|MOVE|4|4|"    
    sleep 7
} | ./rawc localhost $PORT &

sleep 40  #offset to allow game to finish

kill $SERVER_PID 2>/dev/null

echo "=== Test Completed ==="