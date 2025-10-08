#!/bin/bash

rm -f server client

gcc -o server Server2.c
gcc -o client Client2.c

clear