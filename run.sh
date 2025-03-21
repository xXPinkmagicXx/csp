#!/bin/bash

# Define source file and output binary
SOURCE="main.cpp abstract_method.cpp concurrent_method.cpp independent_method.cpp"
OUTPUT="out.o"
FLAGS="-pthread"
USE_AFFINITY=0

if [ -n "$1" ]; then
    USE_AFFINITY=1
fi

# Compile the C++ program
g++ -o $OUTPUT $SOURCE $FLAGS

# Define result directory
RESULTS_DIR="results"
TIMESTAMP=$(date +"%Y-%m-%d_%H-%M-%S")
echo "Current timestamp: $TIMESTAMP"

# Does file exist?
if [ "$USE_AFFINITY" -eq 1 ]; then
    if [ ! -f "affinity/$1.txt" ]; then
        echo "Error: affinity/"$1.txt" not found"
        exit 1
    fi
fi

# Create result directory if it doesn't exist
if [ ! -d $RESULTS_DIR ]; then
   mkdir $RESULTS_DIR
fi

mkdir $RESULTS_DIR/$TIMESTAMP

if [ "$USE_AFFINITY" -eq 1 ]; then
    cp affinity/$1.txt $RESULTS_DIR/$TIMESTAMP
fi

# Define result file prefixes
INDEPENDENT_RESULTS_PREFIX="./$RESULTS_DIR/$TIMESTAMP/independent"
CONCURRENT_RESULTS_PREFIX="./$RESULTS_DIR/$TIMESTAMP/concurrent"

## Create results files
for i in {0..5}
do
    NUM_THREAD=$((2**$i))
    if [ "$USE_AFFINITY" -eq 1 ]; then
        IND_RESULTS_FILE="$INDEPENDENT_RESULTS_PREFIX"_"$1"_"$NUM_THREAD.csv"
        CON_RESULTS_FILE="$CONCURRENT_RESULTS_PREFIX"_"$1"_"$NUM_THREAD.csv"
    else
        IND_RESULTS_FILE="$INDEPENDENT_RESULTS_PREFIX"_"$NUM_THREAD.csv"
        CON_RESULTS_FILE="$CONCURRENT_RESULTS_PREFIX"_"$NUM_THREAD.csv"
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
        if [ "$USE_AFFINITY" -eq 1 ]; then
            ./$OUTPUT $j $NUM_THREAD 0 0 $1 >> $INDEPENDENT_RESULTS_PREFIX"_"$1"_"$NUM_THREAD.csv
        else
            ./$OUTPUT $j $NUM_THREAD 0 0 $1 >> $INDEPENDENT_RESULTS_PREFIX"_"$NUM_THREAD.csv
        fi
    done
done

#Run concurrent method
for i in {0..5}
do
    NUM_THREAD=$((2**$i)) # number of threads
    for j in {1..18} # hashbits
    do
        echo "Running concurrent method with $NUM_THREAD threads and $j hashbits"
        if [ "$USE_AFFINITY" -eq 1 ]; then
            ./$OUTPUT $j $NUM_THREAD 0 1 $1 >> $CONCURRENT_RESULTS_PREFIX"_"$1"_"$NUM_THREAD.csv
        else
            ./$OUTPUT $j $NUM_THREAD 0 1 $1 >> $CONCURRENT_RESULTS_PREFIX"_"$NUM_THREAD.csv
        fi
    done
done

if [ "$USE_AFFINITY" -eq 1 ]; then
    python3 graph.py $TIMESTAMP $1
else
    python3 graph.py $TIMESTAMP
fi