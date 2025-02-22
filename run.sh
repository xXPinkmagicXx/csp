#!/bin/bash

# Define source file and output binary
# SOURCE="main.cpp"
OUTPUT="out.o"

# Compile the C++ program
# g++ -o $OUTPUT $SOURCE

# Check if compilation was successful
for i in {2..4} # hashbits
do
    for j in {0..5} # t
    do
        ./$OUTPUT $i 2$j 0
    done
done