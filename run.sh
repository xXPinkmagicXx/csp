#!/bin/bash

# Define source file and output binary
SOURCE="main.cpp"
OUTPUT="out.o"
FLAGS="-pthread"

# Compile the C++ program
# g++ -o $OUTPUT $SOURCE
BASE_RESULTS_FILE="results.csv"

# Create the results files
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
echo "hash_bits,mil_tup_per_sec" >> $INDEPENDENT_RESULTS_FILE
touch $CONCURRENT_RESULTS_FILE
echo "hash_bits,mil_tup_per_sec" >> $CONCURRENT_RESULTS_FILE

## Create results files
for i in {1..18} # hashbits
do
    NUM_THREAD=$((2**$i))
    IND_RESULTS_FILE="$NUM_THREAD"_"$INDEPENDENT_RESULTS_FILE"
    CON_RESULTS_FILE="$NUM_THREAD"_"$CONCURRENT_RESULTS_FILE"
    # Remove old independent results file
    if [ -f "results/$IND_RESULTS_FILE" ]; then
        rm $IND_RESULTS_FILE
        echo "Removed old results file: $IND_RESULTS_FILE"
    fi
    ## Remove old concurrent results file
    if [ -f "results/$CON_RESULTS_FILE" ]; then
        rm $CON_RESULTS_FILE
        echo "Removed old results file: $CON_RESULTS_FILE"
    fi

    mkdir -p results # Make dir if it doesn't exist
    touch "results/$CON_RESULTS_FILE"
    touch "results/$IND_RESULTS_FILE"
    echo "hash_bits,mil_tup_per_sec" >> $CON_RESULTS_FILE
    echo "hash_bits,mil_tup_per_sec" >> $IND_RESULTS_FILE
done

# #Run independent method
# for i in {0..5} # number of threads 
# do
#     NUM_THREAD=$((2**$i))
#     for j in {1..18} # hashbits
#     do
#         ./$OUTPUT $i $((2**$j)) 1 0
#     done
# done
# #Run concurrent method
# for i in {1..18} # hashbits
# do
#     # -n flag to not print newline
#     for j in {0..5} # number of threads 
#     do
#         ./$OUTPUT $i $((2**$j)) 1 1
#     done
# done
