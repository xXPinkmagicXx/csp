#!/bin/bash

# Define source file and output binary
# SOURCE="main.cpp"
OUTPUT="out.o"

# Compile the C++ program
# g++ -o $OUTPUT $SOURCE
RESULTS_FILE="results.txt"

# Check if compilation was successful
if [ -f $RESULTS_FILE ]; then
   rm $RESULTS_FILE
   echo "Removed old results file"
fi

for i in {2..4} # hashbits
do
    for j in {0..5} # number of threads 
    do
        ./$OUTPUT $i 2^$j 1
    done
done