#!/bin/bash

# Define source file and output binary
SOURCE="main.cpp abstract_method.cpp concurrent_method.cpp independent_method.cpp"
OUTPUT="out.o"
FLAGS="-pthread"

# Compile the C++ program
g++ -o $OUTPUT $SOURCE $FLAGS

# Define result directory
RESULTS_DIR="results"

# Does file exist?
if [ ! -f "affinity/$1.txt" ]; then
    echo "Error: affinity/"$1.txt" not found"
    exit 1
fi

# Create result directory if it doesn't exist
if [ ! -d $RESULTS_DIR ]; then
   mkdir $RESULTS_DIR
fi

# Define result file prefixes
INDEPENDENT_RESULTS_PREFIX="./$RESULTS_DIR/independent"
CONCURRENT_RESULTS_PREFIX="./$RESULTS_DIR/concurrent"

## Create results files
for i in {0..5}
do
    NUM_THREAD=$((2**$i))
    IND_RESULTS_FILE="$INDEPENDENT_RESULTS_PREFIX"_"$1"_"$NUM_THREAD.csv"
    CON_RESULTS_FILE="$CONCURRENT_RESULTS_PREFIX"_"$1"_"$NUM_THREAD.csv"
    # Remove old independent results file
    if [ -f $IND_RESULTS_FILE ]; then
        rm $IND_RESULTS_FILE
        echo "Removed old results file: $IND_RESULTS_FILE"
    fi
    ## Remove old concurrent results file
    if [ -f $CON_RESULTS_FILE ]; then
        rm $CON_RESULTS_FILE
        echo "Removed old results file: $CON_RESULTS_FILE"
    fi
    touch $IND_RESULTS_FILE
    touch $CON_RESULTS_FILE
    echo "hash_bits,mil_tup_per_sec" >> $IND_RESULTS_FILE
    echo "hash_bits,mil_tup_per_sec" >> $CON_RESULTS_FILE
done

# #Run independent method
for i in {0..5}
do
    NUM_THREAD=$((2**$i)) # number of threads
    for j in {1..18} # hashbits
    do
        echo "Running independent method with $NUM_THREAD threads and $j hashbits" 
        ./$OUTPUT $j $NUM_THREAD 1 0 $1 
    done
done

#Run concurrent method
for i in {0..5}
do
    NUM_THREAD=$((2**$i)) # number of threads
    for j in {1..18} # hashbits
    do
        echo "Running concurrent method with $NUM_THREAD threads and $j hashbits" 
        ./$OUTPUT $j $NUM_THREAD 1 1 $1
    done
done