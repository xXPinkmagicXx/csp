# Computer System Performance (CSP)

## How to compile the code

Compile the main script (remember to use `-pthread`)
```bash
g++ -o out.o main.cpp abstract_method.cpp concurrent_method.cpp independent_method.cpp -pthread
```

Compile the generate script
```bash
g++ -o generate.o generate.cpp
```

## Results

results are in `results.csv` file 
The columns are
- hash_bits
- num_threads
- mil_tup_per_sec

## How to run the code

### Run experiment
Run the bash script and regenerate the `results.csv` file
```bash
./run.sh
```

### Run main
Arguments
1. hashbits=4
2. number of threads=4
3. verbose=0 {0,1,2}  
    0 = no printing  
    1 = show progress  
    2 = additional information  

```bash
./out.o <hashbits> <number_of_threads> <verbose>
```
This will run out.o with 4 hashbits and 8 threads without printing
```bash
./out.o 4 8 false
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