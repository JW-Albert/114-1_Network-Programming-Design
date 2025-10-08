#!/bin/bash

rm server
rm client

gcc -o server Server2.c
gcc -o client Client2.c