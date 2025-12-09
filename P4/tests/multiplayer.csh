#!/bin/bash
# tests game setup with 4 multiple clients connecting concurrently

echo "=== Testing 2 Concurrent Games Setup ==="

PORT=8081
./nimd $PORT &
SERVER_PID=$!
sleep 2

# Just test that multiple games can start
{
    echo "0|11|OPEN|ALICE|"
    sleep 2
} | ./rawc localhost $PORT &
{
    echo "0|09|OPEN|BOB|"
    sleep 4
} | ./rawc localhost $PORT &
{ 
    echo "0|13|OPEN|CHARLIE|" 
    sleep 6
} | ./rawc localhost $PORT &
{
    echo "0|10|OPEN|DAVE|"
    sleep 8
} | ./rawc localhost $PORT &

sleep 10
kill $SERVER_PID

echo "=== Test Completed ==="