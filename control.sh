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
    NUM_THREADS=4
    METHOD="1" # 0=concurrent;1=independent 
    VERBOSE="2"
    AFFINITY_METHOD="no_affinity"
    echo "Running: out.o $HASH_BITS $NUM_THREADS $VERBOSE $METHOD $AFFINITY_METHOD"
    ./out.o $HASH_BITS $NUM_THREADS $VERBOSE $METHOD $AFFINITY_METHOD

elif [ $1 = "run" ]; then
    echo "Running: run.sh"
    ./run.sh $2     

elif [ $1 = "multi" ] || [ $1 = "multi_numa" ]; then
    echo "Running: run_multi_numa "
    ./run_multi_numa.sh

elif [ $1 = "hyper"] || [ $1 = "hyper_threading" ]; then
    echo "Running: run_hyper_threading "
    ./run_hyper_threading.sh

elif [ $1 = "one" ] || [ $1 = "numa_0" ]; then
    ./run_numa_0.sh

elif [ $1 = "clean" ]; then
    # Remove the compiled program
    rm $OUTPUT
    echo "Removed: $OUTPUT"
else
    echo "Invalid argument: $1"
fi
