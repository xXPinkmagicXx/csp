#!/bin/bash

# Define source file and output binary
# SOURCE="main.cpp"
OUTPUT="out.o"

# Compile the C++ program
# g++ -o $OUTPUT $SOURCE
RESULTS_FILE="results.csv"

# Delete the results file if it exists
if [ -f $RESULTS_FILE ]; then
   rm $RESULTS_FILE
   echo "Removed old results file"
fi

# Create the results file
touch $RESULTS_FILE
echo "hash_bits, num_threads, mil_tup_per_sec" >> $RESULTS_FILE

for i in {1..18} # hashbits
do
    # -n flag to not print newline
    for j in {0..5} # number of threads 
    do
        ./$OUTPUT $i $((2**$j)) 1
    done
done