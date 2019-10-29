#!/bin/bash
# simple build script to make an executable and clean up the object files
gcc -c *.c
gcc -o fm_demo *.o -lusb-1.0
rm *.o
