SOURCE="main.cpp abstract_method.cpp concurrent_method.cpp independent_method.cpp"
OUTPUT="out.o"
FLAGS="-pthread"

if [ $1 = "build" ]; then
    # Compile the C++ program
    g++ -o $OUTPUT $SOURCE $FLAGS
    echo "Compiled: $OUTPUT"
# For testing
elif [ $1 = "test" ]; then
    HASH_BITS=2
    NUM_THREADS=16
    METHOD="1"
    VERBOSE="2"
    AFFINITY_FILE="affinity/linear.txt"
    echo "Running: out.o $HASH_BITS $NUM_THREADS $VERBOSE $METHOD $AFFINITY_FILE"
    ./out.o $HASH_BITS $NUM_THREADS $VERBOSE $METHOD $AFFINITY_FILE

elif [ $1 = "run" ]; then
    echo "Running: run.sh"
    ./run.sh     
else
    echo "Invalid argument: $1"
fi
