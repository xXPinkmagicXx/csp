#!/bin/bash

# Define source file and output binary
# SOURCE="main.cpp"
OUTPUT="out.o"

# Compile the C++ program
# g++ -o $OUTPUT $SOURCE
BASE_RESULTS_FILE="results.csv"

# Create the results files
for i in {0..5} # number of threads 
do
    NUM_THREAD=$((2**$i))
    RESULTS_FILE="$NUM_THREAD"_"$BASE_RESULTS_FILE"
    # Delete the results file if it exists
    if [ -f "results/$RESULTS_FILE" ]; then
        rm $RESULTS_FILE
        echo "Removed old results file: $RESULTS_FILE"
    fi
    mkdir -p results
    touch "results/$RESULTS_FILE"
    echo "hash_bits,mil_tup_per_sec" >> $RESULTS_FILE
done

for i in {0..5} # number of threads 
do
    NUM_THREAD=$((2**$i))
    for j in {1..18} # hashbits
    do
        ./$OUTPUT $j $((2**$i)) 0
    done
done
