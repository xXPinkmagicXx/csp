#include "concurrent_method.h"
#include <cmath>
#include <pthread.h>
#include <future>

using namespace std;

void ConcurrentMethod::init_mutexes() {
    if (VERBOSE == 2)
    cerr << "Initializing mutexes" << endl;
    mutexes = new mutex[get_num_partitions()];
}

void ConcurrentMethod::init_buffers() {
    if (VERBOSE == 2) {
        cerr << "Initializing buffers..." << endl;
    }
    concurrent_buffers = vector<vector<tuple<uint64_t, uint64_t>>>(get_num_partitions());
}

void ConcurrentMethod::work(int thread_index, const vector<tuple<uint64_t, uint64_t>>& data, int start_index, int bucket_size) {

    // Identify the partition by hash function
    barrier->wait();
    if (VERBOSE == 2) {
        cerr << "Thread #" << thread_index << " Started... " << endl;
    }
    for (int i = start_index; i < start_index + bucket_size; i++) {

        // Hash key to get the partition key
        auto key = get<0>(data[i]);
        auto partition_key = hash_function(key);

        // Add tuple to buffer (using locks)
        add_tuple_to_buffer(partition_key, data[i]);
    }

    barrier->wait();
    if (VERBOSE == 2)
        cerr << "Thread #" << thread_index << " completed... " << endl;
}

double ConcurrentMethod::thread_work_affinity(const vector<tuple<uint64_t, uint64_t>>& data){

    // Initialize mutexes for each partition
    init_mutexes();
    init_buffers();

    // Create buffers for each partition
    auto bucket_size = data.size() / NUM_THREADS;
    if (VERBOSE == 2) {
        cerr << "Starting with " << NUM_THREADS << " threads and bucket size " << bucket_size << endl;
    }

    auto is_affinity_valid = read_affinity_file();

    if (!is_affinity_valid) {
        cerr << "Affinity file is not valid" << endl;
        exit(1);
    }
   
    // Initialize threads
    vector<thread> threads(NUM_THREADS);
    for (int i = 0; i < NUM_THREADS; ++i) {
        
        //cerr << "before init thread #" << i << endl;
        threads[i] = thread(&ConcurrentMethod::work, this, i, cref(data), i * bucket_size, bucket_size);
        //cerr << "after init thread #" << i << endl;
        
        cpu_set_t cpuset;
        CPU_ZERO(&cpuset);
        CPU_SET(affinity[i], &cpuset);
        int rc = pthread_setaffinity_np(threads[i].native_handle(), sizeof(cpu_set_t), &cpuset);
        
        if (VERBOSE == 2) {
            cerr << "Thread #" << i << " @ " << affinity[i] <<  "- start_index: " << i * bucket_size << endl;
        }
        if (rc != 0) {
            cerr << "Error calling pthread_setaffinity_np: " << rc << endl;
        }
    }

    

    // Join and start threads
    // For i threads
    for(int i = 0; i < NUM_THREADS; i++) {
        //cerr << "before Joining thread #" << i << endl;
        threads[i].detach();
        //cerr << "after Joining thread #" << i << endl;
    }

    barrier->wait();
    // Start times here
    //cerr << "----Before Timer start----" << endl;
    auto start_time = chrono::high_resolution_clock::now();
    //cerr << "----After Timer start----" << endl;
    
    
    barrier->wait();
    //cerr << "----Before Timer end----" << endl;
    // End timer and calculate duration
    auto end_time = chrono::high_resolution_clock::now();
    int64_t duration = chrono::duration_cast<chrono::microseconds>(end_time - start_time).count();
    //cerr << "----After Timer end----" << endl;

    // Return time (convert from microseconds to milliseconds)
    return static_cast<double>(duration)/1000;
}


double ConcurrentMethod::thread_work(const vector<tuple<uint64_t, uint64_t>>& data) {
    // Initialize mutexes for each partition
    init_mutexes();
    init_buffers();

    // Create buffers for each partition
    auto bucket_size = data.size() / NUM_THREADS;
    if (VERBOSE == 2) {
        cerr << "Starting with " << NUM_THREADS << " threads and bucket size " << bucket_size << endl;
    }
    
    // Initialize threads
    vector<thread> threads(NUM_THREADS);
    for (int i = 0; i < NUM_THREADS; ++i) {
        
        //cerr << "before init thread #" << i << endl;
        threads[i] = thread(&ConcurrentMethod::work, this, i, cref(data), i * bucket_size, bucket_size);
        //cerr << "after init thread #" << i << endl;
        
        if (VERBOSE == 2) {
            cerr << "Thread #" << i <<  "- start_index: " << i * bucket_size << endl;
        }
    }

    

    // Join and start threads
    // For i threads
    for(int i = 0; i < NUM_THREADS; i++) {
        //cerr << "before Joining thread #" << i << endl;
        threads[i].detach();
        //cerr << "after Joining thread #" << i << endl;
    }

    barrier->wait();
    // Start times here
    //cerr << "----Before Timer start----" << endl;
    auto start_time = chrono::high_resolution_clock::now();
    //cerr << "----After Timer start----" << endl;
    
    
    barrier->wait();
    //cerr << "----Before Timer end----" << endl;
    // End timer and calculate duration
    auto end_time = chrono::high_resolution_clock::now();
    int64_t duration = chrono::duration_cast<chrono::microseconds>(end_time - start_time).count();
    //cerr << "----After Timer end----" << endl;

    // Return time (convert from microseconds to milliseconds)
    return static_cast<double>(duration)/1000;
}

void ConcurrentMethod::add_tuple_to_buffer(int partition_key, tuple<uint64_t, uint64_t> tuple_to_add) {
    // Add data to buffer
    // Lock the mutex
    mutexes[partition_key].lock();
    // cerr << "Locked partition: " << partition_key << endl;
    concurrent_buffers[partition_key].push_back(tuple_to_add);
    mutexes[partition_key].unlock();
    // cerr << "Unlocked partition: " << partition_key << endl;
}

void ConcurrentMethod::print_buffers_everything() {
    for (int i = 0; i < concurrent_buffers.size(); i++) {
        for (int j = 0; j < concurrent_buffers[i].size(); j++) {
            cerr << "Partition: " << i << " Key: " << get<0>(concurrent_buffers[i][j])
                << " Value: " << get<1>(concurrent_buffers[i][j]) << endl;
        }
    }
}

void ConcurrentMethod::print_buffers_partition_entries() {
    for (int i = 0; i < concurrent_buffers.size(); i++) {
        auto partition_size = concurrent_buffers[i].size();
        // counter_arr[i] = partition_size;
        cerr << "Partition: " << i << "# entries: " << partition_size << endl;
    }
}

void ConcurrentMethod::print_buffers_partition_statistics() {
    int num_partitions = get_num_partitions();
    //auto counter_arr = new int[num_partitions];

    float mean = DATA_SIZE / num_partitions;
    float std_dev = 0.0;

    for (int i = 0; i < concurrent_buffers.size(); i++) {
        auto partition_size = concurrent_buffers[i].size();
        // counter_arr[i] = partition_size;
        auto variance = partition_size - mean;
        std_dev += variance * variance;
    }

    std_dev = sqrt(std_dev / num_partitions);
    if (VERBOSE == 1) {
        cerr << "Expected Partition Size: " << mean << " ±" << std_dev << endl;
    }
}
