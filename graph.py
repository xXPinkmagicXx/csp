import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

def main():
    pass

def read_data():
    pass

def generate_graph(all_results: list[pd.DataFrame], num_threads: list[int]):
    ## Save graph to file
    # fig = plt.figure(figsize=(10, 6))
    for i in range(len(num_threads)):
        xs = all_results[i]["hash_bits"]
        ys = all_results[i]["mil_tup_per_sec"]
        plt.plot(xs, ys, marker='o', label=f"{num_threads[i]} threads")
    plt.legend()
    plt.xlabel("Hash bits")
    ## xticks even only 
    plt.xticks(range(0, 20, 2))
    plt.xticks()
    plt.ylabel("Million tuples per second")
    plt.savefig("results/fig.png")

if __name__ == '__main__':
    main()