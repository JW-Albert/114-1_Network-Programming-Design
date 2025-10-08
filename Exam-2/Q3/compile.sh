#!/bin/bash

echo "Removing old server and client..."
rm -f server client

echo "Compiling server..."
gcc -o server Server3.c
echo "Server compiled"

echo "Compiling client..."
gcc -o client Client3.c
echo "Client compiled"

echo "Done"
