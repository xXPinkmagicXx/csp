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

### Run All Experiment (recommended)
Run the bash script and regenerate the `results.csv` file
```bash
./run.sh
```

### Run graph.py
Run `graph.py` to generate the two graphs `concurrent_fig.png` and `independent_fig.png`

Note: it is assumed that `./run.sh` has been run before generating the graphs

```bash
python graph.py
```

This will run the block inside `if __name__ == '__main__'`

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

### Running experiment on remote server using TMUX

Create a new tmux session with
```bash
tmux new-session -s latencylegends12
```

Once inside the session, run the shell script
```bash
./run.sh
```

To detach from an active session, press **Ctrl + B**, then release both keys, then press **D**.

To list active sessions, write
```bash
tmux ls
```

To reattach to a running/active session, write
```bash
tmux attach -t latencylegends12
```

You can kill the session from outside with
```bash
tmux kill-session -t latencylegends12
```
or alternatively, write 'exit' inside the session.

To copy the results to your Desktop
```bash
scp -r group12@dionysos.itu.dk:/home/group12/csp/results ~/Desktop/
```
