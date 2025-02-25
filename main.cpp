#include "concurrent_method.h"
#include "independent_method.h"
#include <iostream>
#include <tuple>
#include <fstream>
#include <vector>
#include <thread>
#include <chrono>

using namespace std;

const string fileName = "random_integers.txt";
const string resultsFile = "results.csv";
int HASH_BITS = 4;
int NUM_THREADS = 4;
int DATA_SIZE = 0;
int verbose = 0;

// Cast one string line to a unsigned 64-bit integer tuplet  
tuple<uint64_t, uint64_t> cast_to_tuple(string line) {
    size_t pos = line.find(' ');
    
    uint64_t first = stoull(line.substr(0, pos));
    uint64_t second = stoull(line.substr(pos + 1));

    tuple<uint64_t, uint64_t> result(first, second);
    
    return result;
}

// Read generated file with random tuples of unsigned 64-bit integers
vector<tuple<uint64_t, uint64_t>> read_file() {
    ifstream file(fileName);
    string str;
    vector<tuple<uint64_t, uint64_t>> tuples;
    while (getline(file, str))
    {
        auto tuple = cast_to_tuple(str);
        tuples.push_back(tuple);
    }
    return tuples;
}

bool is_power_of_two(ulong x) {
    return (x & (x - 1)) == 0;
}


void write_results_to_file(float million_tuples_per_second) {
    ofstream file;
    file.open(resultsFile, ios::app);
    // Write ulong to file
    file << HASH_BITS << "," << NUM_THREADS << "," << million_tuples_per_second << endl;
    file.close();
}

int main(int argc, char *argv[]) {
    
    if(argc <= 2 ) {
        cout << "Using default values hash_bits=" << HASH_BITS << " thread_count=" << NUM_THREADS << endl;
    } else {
        // Get and validate the hash bits argument 
        int hash_bit_arg = stoi(argv[1]);
        if(hash_bit_arg <= 0 || hash_bit_arg > 18) {
            cout << "First arg: enter a positive number between 1-18" << endl;
            return 1;
        }
        
        // Get and validate the # threads argument
        int thread_count_arg = stoi(argv[2]);
        if(thread_count_arg <= 0 || thread_count_arg > 32) {
            cout << "Second arg: enter positive number between 1-32" << endl;
            return 1;
        }
        // Get and validate the verbose argument
        int verbose_arg = 0;
        if (argc == 4 && verbose_arg >= 0 && verbose_arg < 3) {
            verbose_arg = stoi(argv[3]); 
        }
        verbose = verbose_arg;
        HASH_BITS = hash_bit_arg;
        NUM_THREADS = thread_count_arg;
    }
    
    
    // Read data from file
    // Note that the data is/should be read only
    auto data = read_file();    

    auto data_size = data.size();
    if(verbose == 2) {
        cout << "Data size: " << data_size << endl;
    }
    if(data_size == 0) {
        cout << "No data to process" << endl;
        cout << "Closing..." << endl;
        return 1;
    } 
    
    if(data_size < NUM_THREADS) {
        cout << "Data size is less than the number of threads"  << endl;
        cout << "Data size:" << data_size << "# threads" << NUM_THREADS << endl;
        cout << "Closing..." << endl;
        return 1;
    }

    if(!is_power_of_two(data_size)) {
        cout << "Data size is not a power of 2" << endl;
        cout << "Use generate.o to generate data" << endl;
        cout << "Closing..." << endl;
        return 1;
    }

    DATA_SIZE = data_size;
    // print_hash_values(data);

    auto start_time = chrono::high_resolution_clock::now();

    // Do the work in threads
    //ConcurrentMethod concurrent_method(HASH_BITS, NUM_THREADS, DATA_SIZE, verbose);
    //concurrent_method.thread_work(data);

    IndependentMethod independent_method(HASH_BITS, NUM_THREADS, DATA_SIZE, verbose);
    independent_method.thread_work(data);

    auto end_time = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end_time - start_time).count();
    
    // print summary
    float tuples_pr_ms = data_size / duration;
    float tuples_per_second = tuples_pr_ms * 1000;
    float million_tuples_per_second = tuples_per_second / 1000000;
    write_results_to_file(million_tuples_per_second);
    if(verbose == 1) {
        //concurrent_method.print_buffers_partition_statistics();
        independent_method.print_buffers_partition_statistics();
        // cout << "Time taken: " << duration << " milliseconds" << endl;
        cout << "Million Tuples per second: " << million_tuples_per_second << endl; 
    }


    return 0;
}

