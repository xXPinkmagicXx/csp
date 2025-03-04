SOURCE="main.cpp abstract_method.cpp concurrent_method.cpp independent_method.cpp"
OUTPUT="out.o"
FLAGS="-pthread"

if [ $1 = "build" ]; then
    # Compile the C++ program
    g++ -o $OUTPUT $SOURCE $FLAGS
fi

if [ $1 = "test" ]; then
    HASH_BITS=2
    NUM_THREADS=4
    METHOD=1
    VERBOSE=2
    ./out.o $HASH_BITS $NUM_THREAD $VERBOSE $METHOD
fi