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
std::mutex *mutexes;
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
    cout << "Initializing mutexes" << endl;
    mutexes = new std::mutex[get_num_partitions()];
}

void init_buffers() {
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
    cout << "Thread #" << thread_index << ": start_index= " << start_index << endl;
    // Identify the partition by hash function
    for(int i = start_index; i < start_index + bucket_size; i++){
        // Hash key to get the partition key
        auto key = get<0>(data[i]);
        auto partition_key = hash_function(key);
        
        // Add tuple to buffer (using locks)
        add_tuple_to_buffer(partition_key, data[i]);
    }
    cout << "Thread #" << thread_index << " completed... "<< endl;
}

void thread_work(vector<tuple<uint64_t, uint64_t>> data) {
    
    // Initialize mutexes for each partition
    init_mutexes();
    init_buffers();
    // Create buffers for each partition
    auto bucket_size = data.size() / NUM_THREADS;
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

void print_buffers() {
    for(int i = 0; i < concurrent_buffers.size(); i++){
        for(int j = 0; j < concurrent_buffers[i].size(); j++){
            cout << "Partition: " << i << " Key: " << get<0>(concurrent_buffers[i][j]) << " Value: " << get<1>(concurrent_buffers[i][j]) << endl;
        }
    }
}

int main(int argc, char *argv[]) {
    int hash_bit_arg;
    int thread_count_arg;
    if(argc <= 2 ) {
        cout << "Using default values hash_bits=" << HASH_BITS << " thread_count=" << NUM_THREADS << endl;
    } else {
        hash_bit_arg = stoi(argv[1]);
        thread_count_arg = stoi(argv[2]);
        
        if(hash_bit_arg <= 0 || hash_bit_arg > 18) {
            cout << "Please enter a positive number between 1-18" << endl;
            return 1;
        }
    
        if(thread_count_arg <= 0 || thread_count_arg > 32) {
            cout << "Please enter a positive number between 1-32" << endl;
            return 1;
        }
        
        HASH_BITS = hash_bit_arg;
        NUM_THREADS = thread_count_arg;
    }
    
    
    // Read data from file
    // Note that the data is/should be read only
    auto data = read_file();    

    cout << "Data size: " << data.size() << endl;
    auto data_size = data.size();
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

    // print_hash_values(data);

    // Do the work in threads
    thread_work(data);

    print_buffers();

    return 0;
}

