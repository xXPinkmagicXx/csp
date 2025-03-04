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
## Affinity
- All in one numa node
- All different cores (no hyper threading)
- No config

## Perf
- Context switches
- Cache misses


## Results

The results are saved in the folder `results/`  
Files are formatted: `<methods>_<#threads>.csv`  
Each file contains
- hash_bits
- mil_tup_per_sec

## Python Environment (Python=3.12.9)

Create env with conda or python

### Install nessesary requirments to run python script
```bash
pip install -r requirements.txt
```

### Updating requirements.txt

The easiest way is to use `pipreqs`. Install by `pip install pipreqs`   
```
pipreqs . --force
```
Note: 
- `--force` overrides the current `requirements.txt` file.
- `.` (the dot in the code) is the path to the project. So "." is the current folder


## How to run the code

### Run via Control 

Compile, test and run via `control.sh`
You can pass in
- run: runs `./run.sh`
- test: run `out.o` with default args
- build: compiles the code

```bash
./control.sh <function>
```

```bash
./control.sh build
Compiled: out.o
```


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
4. method {0,1}
5. path_to_affinity_file

```bash
./out.o <hashbits> <number_of_threads> <verbose> <method> <path_to_affinity_file>
```
This will run out.o with 4 hashbits and 8 threads without printing
```bash
./out.o 4 8 0 1 affinity/numa_0.txt
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
