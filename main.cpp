#include "concurrent_method.h"
#include "independent_method.h"
#include "abstract_method.h"
#include <tuple>
#include <fstream>
#include <vector>
#include <thread>
#include <chrono>

using namespace std;

const string input_file = "random_integers.txt";
const string output_dir = "./results/";
const string output_file_extension = ".csv";
int HASH_BITS = 4;
int NUM_THREADS = 4;
int VERBOSE = 0;
int method_type = 0;
mutex fileMutex;

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
    ifstream file(input_file);
    string str;
    vector<tuple<uint64_t, uint64_t>> tuples;
    while (getline(file, str))
    {
        tuple<uint64_t, uint64_t> tuple = cast_to_tuple(str);
        tuples.push_back(tuple);
    }
    return tuples;
}

bool is_power_of_two(uint64_t x) {
    return (x & (x - 1)) == 0;
}

void write_results_to_file(string path, float million_tuples_per_second) {
    ofstream file;
    file.open(path, ios::app);
    // Write ulong to file
    file << HASH_BITS << "," << million_tuples_per_second << endl;
    file.close();
    fileMutex.unlock();
}

void do_method(AbstractMethod& method, const vector<tuple<uint64_t, uint64_t>>& data, size_t data_size, string output_file_name) {
    auto start_time = chrono::high_resolution_clock::now();

    method.thread_work(cref(data));

    auto end_time = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end_time - start_time).count();
    
    // print summary
    float tuples_pr_ms = data_size / duration;
    float tuples_per_second = tuples_pr_ms * 1000;
    float million_tuples_per_second = tuples_per_second / 1000000;
    write_results_to_file(output_dir + output_file_name + output_file_extension, million_tuples_per_second);
    if(VERBOSE == 1) {
        method.print_buffers_partition_statistics();
        // cout << "Time taken: " << duration << " milliseconds" << endl;
        cout << "Million Tuples per second: " << million_tuples_per_second << endl; 
    }
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
        if (argc == 5) {
            method_type = stoi(argv[4]);
        }
        VERBOSE = verbose_arg;
        HASH_BITS = hash_bit_arg;
        NUM_THREADS = thread_count_arg;
    }
    
    
    // Read data from file
    // Note that the data is/should be read only
    vector<tuple<uint64_t, uint64_t>> data = read_file();    

    uint64_t data_size = data.size();
    if(VERBOSE == 2) {
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
    
    // Do the correct type of partitioning
    if (method_type == 0) {
        IndependentMethod independent_method(HASH_BITS, NUM_THREADS, data_size, VERBOSE);
        do_method(independent_method, data, data_size, "independent_" + to_string(NUM_THREADS));
    } else if (method_type == 1) {
        ConcurrentMethod concurrent_method(HASH_BITS, NUM_THREADS, data_size, VERBOSE);
        do_method(concurrent_method, data, data_size, "concurrent_" + to_string(NUM_THREADS));
    } else {
        cout << "Unknown method type" << endl;
        cout << "Closing..." << endl;
        return 1;
    }

    return 0;
}