# OS_PS

This package includes the following files.

- scheduler.c [This is the main program which will output the info.]
- scheduler.h [Header file declaring the structs used in program.]
- README.md [This file]

To compile:
    make all

To clean:
    make clean

To run:
    ./Scheduler <input file> <random seed>

For example;
    ./Coordinator input1000.txt 1234 > output.txt

To view results: 
    less output.txt
