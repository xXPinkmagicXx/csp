#!/bin/bash

# Define source file and output binary
# SOURCE="main.cpp"
OUTPUT="out.o"

# Compile the C++ program
# g++ -o $OUTPUT $SOURCE
RESULTS_FILE="results.txt"

# Delete the results file if it exists
if [ -f $RESULTS_FILE ]; then
   rm $RESULTS_FILE
   echo "Removed old results file"
fi

# Create the results file
touch $RESULTS_FILE

# Create First line of results file
ALL_THREADS=""
for i in {0..5} # threads
do    
    ALL_THREADS+="$i "
done
echo "$ALL_THREADS" >> $RESULTS_FILE

for i in {2..4} # hashbits
do
    # -n flag to not print newline
    echo -n "$i," >> $RESULTS_FILE
    for j in {0..5} # number of threads 
    do
        ./$OUTPUT $i 2^$j 1
    done
    echo "" >> $RESULTS_FILE
done