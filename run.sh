#!/bin/bash

# Define source file and output binary
SOURCE="main.cpp"
OUTPUT="out.o"
FLAGS="-pthread"

# Compile the C++ program
g++ -o $OUTPUT $SOURCE $FLAGS

# Define result directory
RESULTS_DIR="results"

# Create result directory if it doesn't exist
if [ ! -d $RESULTS_DIR ]; then
   mkdir $RESULTS_DIR
fi

# Define result files
INDEPENDENT_RESULTS_FILE="./$RESULTS_DIR/independent_results.csv"
CONCURRENT_RESULTS_FILE="./$RESULTS_DIR/concurrent_results.csv"

# Delete the results files if they exists
if [ -f $INDEPENDENT_RESULTS_FILE ]; then
   rm $INDEPENDENT_RESULTS_FILE
   echo "Removed old 'Independent' results file"
fi
if [ -f $CONCURRENT_RESULTS_FILE ]; then
   rm $CONCURRENT_RESULTS_FILE
   echo "Removed old 'Concurrent' results file"
fi

# Create the results files
touch $INDEPENDENT_RESULTS_FILE
echo "hash_bits, num_threads, mil_tup_per_sec" >> $INDEPENDENT_RESULTS_FILE
touch $CONCURRENT_RESULTS_FILE
echo "hash_bits, num_threads, mil_tup_per_sec" >> $CONCURRENT_RESULTS_FILE

#Run independent method
for i in {1..18} # hashbits
do
    # -n flag to not print newline
    for j in {0..5} # number of threads 
    do
        ./$OUTPUT $i $((2**$j)) 1 0
    done
done
#Run concurrent method
for i in {1..18} # hashbits
do
    # -n flag to not print newline
    for j in {0..5} # number of threads 
    do
        ./$OUTPUT $i $((2**$j)) 1 1
    done
done