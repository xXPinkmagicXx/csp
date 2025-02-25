#!/bin/bash

# Define source file and output binary
# SOURCE="main.cpp"
OUTPUT="out.o"

# Compile the C++ program
# g++ -o $OUTPUT $SOURCE
BASE_RESULTS_FILE="results.csv"

# Delete the results file if it exists


# Create the results file
# touch $RESULTS_FILE
# echo "hash_bits, num_threads, mil_tup_per_sec" >> $RESULTS_FILE

# # -n flag to not print newline
for i in {0..5} # number of threads 
do
    NUM_THREAD=$((2**$i))
    RESULTS_FILE="$NUM_THREAD"_"$BASE_RESULTS_FILE"
    if [ -f $RESULTS_FILE ]; then
        rm $RESULTS_FILE
        echo "Removed old results file: $RESULTS_FILE"
    fi
    touch "$NUM_THREAD"_"$BASE_RESULTS_FILE"
    for j in {1..18} # hashbits
    do
        ./$OUTPUT $j $((2**$j)) 0
    done
done
