#!/bin/bash

echo "Removing old server and client..."
rm -f server client

echo "Compiling server..."
gcc -o server Server2.c
echo "Server compiled"

echo "Compiling client..."
gcc -o client Client2.c
echo "Client compiled"

echo "Done"
