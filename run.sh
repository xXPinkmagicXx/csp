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
    
    echo "Running independent method with $NUM_THREAD threads"
    if [ "$USE_AFFINITY" -eq 1 ]; then
        perf stat -e cycles,instructions,L1-icache-load-misses,L1-dcache-load-misses,LLC-load-misses,cache-misses,uops_retired.stall_cycles,branch-misses,iTLB-load-misses,dTLB-load-misses,context-switches -o $INDEPENDENT_RESULTS_PREFIX"_"$1"_"$NUM_THREAD"_perf.txt" ./$OUTPUT 0 $NUM_THREAD 0 0 $1 >> $INDEPENDENT_RESULTS_PREFIX"_"$1"_"$NUM_THREAD.csv
    else
        perf stat -e cycles,instructions,L1-icache-load-misses,L1-dcache-load-misses,LLC-load-misses,cache-misses,uops_retired.stall_cycles,branch-misses,iTLB-load-misses,dTLB-load-misses,context-switches -o $INDEPENDENT_RESULTS_PREFIX"_"$NUM_THREAD"_perf.txt" ./$OUTPUT 0 $NUM_THREAD 0 0 $1 >> $INDEPENDENT_RESULTS_PREFIX"_"$NUM_THREAD.csv
    fi
done

#Run concurrent method
for i in {0..5}
do
    NUM_THREAD=$((2**$i)) # number of threads
    echo "Running concurrent method with $NUM_THREAD threads"
    if [ "$USE_AFFINITY" -eq 1 ]; then
        perf stat -e cycles,instructions,L1-icache-load-misses,L1-dcache-load-misses,LLC-load-misses,cache-misses,uops_retired.stall_cycles,branch-misses,iTLB-load-misses,dTLB-load-misses,context-switches -o $CONCURRENT_RESULTS_PREFIX"_"$1"_"$NUM_THREAD"_perf.txt" ./$OUTPUT 0 $NUM_THREAD 0 1 $1 >> $CONCURRENT_RESULTS_PREFIX"_"$1"_"$NUM_THREAD.csv
    else
        perf stat -e cycles,instructions,L1-icache-load-misses,L1-dcache-load-misses,LLC-load-misses,cache-misses,uops_retired.stall_cycles,branch-misses,iTLB-load-misses,dTLB-load-misses,context-switches -o $CONCURRENT_RESULTS_PREFIX"_"$NUM_THREAD"_perf.txt" ./$OUTPUT 0 $NUM_THREAD 0 1 $1 >> $CONCURRENT_RESULTS_PREFIX"_"$NUM_THREAD.csv
    fi
done

if [ "$USE_AFFINITY" -eq 1 ]; then
    python3 graph.py $RESULTS_DIR/$TIMESTAMP $1
else
    python3 graph.py $RESULTS_DIR/$TIMESTAMP
fi