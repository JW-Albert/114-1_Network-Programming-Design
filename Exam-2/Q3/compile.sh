#!/bin/bash

rm server
rm client

gcc -o server Server3.c

gcc -o client Client3.c
