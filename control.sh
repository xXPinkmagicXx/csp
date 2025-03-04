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
    AFFINITY_METHOD="linear"
    echo "Running: out.o $HASH_BITS $NUM_THREADS $VERBOSE $METHOD $AFFINITY_METHOD"
    ./out.o $HASH_BITS $NUM_THREADS $VERBOSE $METHOD $AFFINITY_METHOD

elif [ $1 = "run" ]; then
    echo "Running: run.sh"
    ./run.sh $2     
else
    echo "Invalid argument: $1"
fi
