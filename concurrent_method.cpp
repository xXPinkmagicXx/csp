#include "concurrent_method.h"
#include <cmath>

using namespace std;

void ConcurrentMethod::init_mutexes() {
    if (VERBOSE == 2)
        cout << "Initializing mutexes" << endl;
    mutexes = new mutex[get_num_partitions()];
}

void ConcurrentMethod::init_buffers() {
    if (VERBOSE == 2)
        cout << "Initializing buffers..." << endl;
    concurrent_buffers = vector<vector<tuple<uint64_t, uint64_t>>>(get_num_partitions());
}

void ConcurrentMethod::work(int thread_index, const vector<tuple<uint64_t, uint64_t>>& data, int start_index, int bucket_size) {
    if (VERBOSE == 2)
        cout << "Thread #" << thread_index << ": start_index= " << start_index << endl;

    // Identify the partition by hash function
    for (int i = start_index; i < start_index + bucket_size; i++) {
        // Hash key to get the partition key
        auto key = get<0>(data[i]);
        auto partition_key = hash_function(key);

        // Add tuple to buffer (using locks)
        add_tuple_to_buffer(partition_key, data[i]);
    }

    if (VERBOSE == 2)
        cout << "Thread #" << thread_index << " completed... " << endl;
}

void ConcurrentMethod::thread_work(const vector<tuple<uint64_t, uint64_t>>& data) {
    // Initialize mutexes for each partition
    init_mutexes();
    init_buffers();

    // Create buffers for each partition
    auto bucket_size = data.size() / NUM_THREADS;
    if (VERBOSE == 2)
        cout << "Starting with " << NUM_THREADS << " threads and bucket size " << bucket_size << endl;

    // Initialize threads
    vector<thread> threads(NUM_THREADS);
    for (int i = 0; i < NUM_THREADS; ++i) {
        threads[i] = thread(&ConcurrentMethod::work, this, i, data, i * bucket_size, bucket_size);
    }

    // Join threads
    for (auto& t : threads) {
        t.join();
    }
}

void ConcurrentMethod::add_tuple_to_buffer(int partition_key, tuple<uint64_t, uint64_t> tuple_to_add) {
    // Add data to buffer
    // Lock the mutex
    mutexes[partition_key].lock();
    // cout << "Locked partition: " << partition_key << endl;
    concurrent_buffers[partition_key].push_back(tuple_to_add);
    mutexes[partition_key].unlock();
    // cout << "Unlocked partition: " << partition_key << endl;
}

void ConcurrentMethod::print_buffers_everything() {
    for (int i = 0; i < concurrent_buffers.size(); i++) {
        for (int j = 0; j < concurrent_buffers[i].size(); j++) {
            cout << "Partition: " << i << " Key: " << get<0>(concurrent_buffers[i][j])
                << " Value: " << get<1>(concurrent_buffers[i][j]) << endl;
        }
    }
}

void ConcurrentMethod::print_buffers_partition_entries() {
    for (int i = 0; i < concurrent_buffers.size(); i++) {
        auto partition_size = concurrent_buffers[i].size();
        // counter_arr[i] = partition_size;
        cout << "Partition: " << i << "# entries: " << partition_size << endl;
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
    cout << "Expected Partition Size: " << mean << " ±" << std_dev << endl;
}
