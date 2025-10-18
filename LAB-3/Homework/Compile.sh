#!/bin/bash

rm -f server client

gcc Server.c -o server -lpthread
gcc Client.c -o client -lpthread