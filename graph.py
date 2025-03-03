import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import platform 
import os
import psutil

NUM_THREADS = [1, 2, 4, 8, 16, 32]

def read_data(method: str) -> list[pd.DataFrame]:
    all_results = []
    for i in NUM_THREADS:
        results = pd.read_csv(f"results/{method}_{i}.csv")
        all_results.append(results)
    return all_results

def read_concurrent_data() -> list[pd.DataFrame]:
    return read_data("concurrent")

def read_independent_data() -> list[pd.DataFrame]:
    return read_data("independent")

def generate_current_graphs() -> None:
    data = read_concurrent_data()
    generate_graph("Concurrent", data)

def generate_independent_graphs()-> None:
    data = read_independent_data()
    generate_graph("Independent", data)

def generate_graph(method: str, all_results: list[pd.DataFrame]) -> None:
    plt.close() # Close the previous figure to avoid overlapping
    plt.rcParams["axes.prop_cycle"] = plt.cycler(
    color=plt.rcParams['axes.prop_cycle'].by_key()['color'][:6],
    marker=['o', 's', 'D', '^', 'v', '*'])
    for i in range(len(NUM_THREADS)):
        xs = all_results[i]["hash_bits"]
        ys = all_results[i]["mil_tup_per_sec"]
        plt.plot(xs, ys, label=f"{NUM_THREADS[i]} threads")
    
    # Get the system and hardware information
    current_processor = platform.processor()
    physical_cores = psutil.cpu_count(logical=False)
    logical_cores = psutil.cpu_count(logical=True)
    system_os = platform.system() # e.g. Windows, Linux
    
    title = f"{method} Method on {system_os} {current_processor} {physical_cores}/{logical_cores} cores"
    plt.title(title)
    plt.legend()
    ## xticks even only 
    plt.xlabel("Hash bits")
    plt.xticks(range(0, 20, 2))
    plt.ylabel("Million tuples per second")
    plt.savefig(f"results/{method.lower()}_fig.png")

def main():
    # When running the script, it will generate the graphs
    generate_current_graphs()
    generate_independent_graphs()

if __name__ == '__main__':
    main()