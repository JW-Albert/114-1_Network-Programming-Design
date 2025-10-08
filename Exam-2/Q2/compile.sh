#!/bin/bash

rm -f server_exec client_exec

gcc -o server_exec Server2.c
gcc -o client_exec Client2.c

clear