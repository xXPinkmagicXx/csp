#include <iostream>
#include <random>
#include <tuple>
#include <fstream>
#include <vector>
#include <thread>
#include <chrono>
#include <mutex>
#include <pthread.h>
#include <atomic>

using namespace std;

const string fileName = "random_integers.txt";
int HASH_BITS = 4;
int NUM_THREADS = 4;
const string baseResultsFile = "results/results.csv";
int DATA_SIZE = 0;
int verbose = 0;
std::mutex *mutexes;
std::mutex fileMutex;
vector<vector<tuple<uint64_t, uint64_t>>> concurrent_buffers; // one buffer per partition 



// std::atomic<int> counter(0);

// Cast one string line to a integer  tupleto  
tuple<uint64_t, uint64_t> cast_to_tuple(std::string line) {
    size_t pos = line.find(' ');
    
    uint64_t first = stoull(line.substr(0, pos));
    uint64_t second = stoull(line.substr(pos + 1));

    tuple<uint64_t, uint64_t> result(first, second);
    
    return result;
}

// Read generated file with random tuples of usigned 64 bit integers
vector<tuple<uint64_t, uint64_t>> read_file() {
    ifstream file(fileName);
    string str;
    vector<tuple<uint64_t, uint64_t>> tuples;
    while (getline(file, str))
    {
        // Process str
        auto tuple = cast_to_tuple(str);
        tuples.push_back(tuple);
    }
    return tuples;
}

int get_num_partitions() {
    return 1 << HASH_BITS;
}

void init_mutexes() {
    if(verbose==2)
        cout << "Initializing mutexes" << endl;
    mutexes = new std::mutex[get_num_partitions()];
}

void init_buffers() {
    if(verbose==2)
        cout << "Initializing buffers..." << endl;
    concurrent_buffers = vector<vector<tuple<uint64_t, uint64_t>>>(get_num_partitions());
}

int hash_function(uint64_t key) {
    // Implement a hash function
    return key % (1 << HASH_BITS);
}

void add_tuple_to_buffer(int partition_key, tuple<uint64_t, uint64_t> tuple) {
    // Add data to buffer
    // Lock the mutex
    mutexes[partition_key].lock();
    // cout << "Locked partition: " << partition_key << endl;
    concurrent_buffers[partition_key].push_back(tuple);
    mutexes[partition_key].unlock();
    // cout << "Unlocked partition: " << partition_key << endl;
}

void print_hash_values(vector<tuple<uint64_t, uint64_t>> data) {
    for(int i = 0; i < data.size(); i++){
        cout << hash_function(get<0>(data[i])) << " " << get<1>(data[i]) << endl;
    }
}
void work(int thread_index, vector<tuple<uint64_t, uint64_t>> data, int start_index, int bucket_size){
    if(verbose==2)
        cout << "Thread #" << thread_index << ": start_index= " << start_index << endl;
    // Identify the partition by hash function
    for(int i = start_index; i < start_index + bucket_size; i++){
        // Hash key to get the partition key
        auto key = get<0>(data[i]);
        auto partition_key = hash_function(key);
        
        // Add tuple to buffer (using locks)
        add_tuple_to_buffer(partition_key, data[i]);
    }
    if(verbose==2)
        cout << "Thread #" << thread_index << " completed... "<< endl;
}

void thread_work(vector<tuple<uint64_t, uint64_t>> data) {
    
    // Initialize mutexes for each partition
    init_mutexes();
    init_buffers();
    // Create buffers for each partition
    auto bucket_size = data.size() / NUM_THREADS;
    if(verbose==2)
        cout << "Starting with " << NUM_THREADS << " threads and bucket size " << bucket_size << endl;
    
    // Initialize threads
    std::vector<std::thread> threads(NUM_THREADS);
    for (int i = 0; i < NUM_THREADS; ++i) {
      threads[i] = thread(work, i, data, i * bucket_size, bucket_size);
    }
    // Join threads
    for (auto& t : threads) {
      t.join();
    }

}

bool is_power_of_two(ulong x){
    return (x & (x - 1)) == 0;
}

void print_buffers_everything() {
    for(int i = 0; i < concurrent_buffers.size(); i++){
        for(int j = 0; j < concurrent_buffers[i].size(); j++){
            cout << "Partition: " << i << " Key: " << get<0>(concurrent_buffers[i][j]) << " Value: " << get<1>(concurrent_buffers[i][j]) << endl;
        }
    }
}

void print_buffers_partition_entries() {
    for(int i = 0; i < concurrent_buffers.size(); i++){
        auto partition_size = concurrent_buffers[i].size();
        // counter_arr[i] = partition_size;
        cout << "Partition: " << i << "# entries: " << partition_size << endl;
    }
}

void print_buffers_partition_statistics() {
    int num_partitions = get_num_partitions();
    auto counter_arr = new int[num_partitions];
    
    float mean = DATA_SIZE / num_partitions;
    float std_dev = 0.0;

    for(int i = 0; i < concurrent_buffers.size(); i++){
        auto partition_size = concurrent_buffers[i].size();
        // counter_arr[i] = partition_size;
        auto variance = partition_size - mean;
        std_dev += variance * variance; 
    }

    std_dev = sqrt(std_dev / num_partitions);
    cout << "Expected Partion Size: " << mean << " ±" << std_dev << endl;
}


void write_results_to_file(const string resultsFile, float million_tuples_per_second) {
    ofstream file;
    if(verbose==2)
        cout << "Writing results to file: " << resultsFile << endl;
    fileMutex.lock();
    file.open(resultsFile, ios::app);
    // Write ulong to file
    file << HASH_BITS << "," << million_tuples_per_second << endl;
    file.close();
    fileMutex.unlock();
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
        if (argc == 4 && verbose_arg >= 0 && verbose_arg < 3){
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
    if(verbose==2)
        cout << "Data size: " << data_size << endl;
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

    if(!is_power_of_two(data_size)){
        cout << "Data size is not a power of 2" << endl;
        cout << "Use generate.o to generate data" << endl;
        cout << "Closing..." << endl;
        return 1;
    }

    DATA_SIZE = data_size;
    auto resultsFile = to_string(NUM_THREADS) + "_" + baseResultsFile;
    // print_hash_values(data);

    auto start_time = chrono::high_resolution_clock::now();

    // Do the work in threads
    thread_work(data);

    auto end_time = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end_time - start_time).count();
    
    // print summary
    float tuples_pr_ms = data_size / duration;
    float tuples_per_second = tuples_pr_ms * 1000;
    float million_tuples_per_second = tuples_per_second / 1000000;
    if(verbose==1){
        // print_buffers_partition_statistics();
        // cout << "Time taken: " << duration << " milliseconds" << endl;
        // cout << "Million Tuples per second: " << million_tuples_per_second << endl; 
    }
    write_results_to_file(resultsFile, million_tuples_per_second);

    return 0;
}

