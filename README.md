# csp
Computer System Performance


## How to compile the code

Compile the main script (remember to use `-pthread`)
```bash
g++ -o out.o main.cpp -pthread
```

Compile the generate script
```bash
g++ -o generate.o generate.cpp
```

## How to run the code

### Run main
Arguments
1. hashbits
2. number of threads

```bash
./out.o <hashbits> <number_of_threads>
```
```bash
./out.o 4 4
```

### Run generate
1. Number of key value pairs to generate
```bash
 ./generate.o <number>
```
Note that you can only generate data with size of the power of 2
```bash
 ./generate.o 8
```