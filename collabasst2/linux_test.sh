#!/bin/bash

rm test.file
gcc -Wall -Werror -O -o run collabasst2.c
./run
