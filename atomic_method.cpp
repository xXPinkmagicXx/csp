#include "atomic_method.h"
#include <cmath>
#include <atomic>

using namespace std;

void AtomicMethod::init_mutexes() {
    if (VERBOSE == 2)
        cout << "Initializing mutexes" << endl;
    mutexes = new mutex[get_num_partitions()];
}

void AtomicMethod::init_buffers() {
    if (VERBOSE == 2)
        cout << "Initializing buffers..." << endl;

    int expected_size = DATA_SIZE / get_num_partitions();
    int max_size = DATA_SIZE;
    concurrent_buffers = vector<vector<tuple<uint64_t, uint64_t>>>(get_num_partitions());
    for(int i = 0; i < get_num_partitions(); i++) {
        auto sized_vector = vector<tuple<uint64_t, uint64_t>>(max_size);
        concurrent_buffers.push_back(sized_vector);
    }
}

void AtomicMethod::init_buffer_index() {
    auto number = get_num_partitions();
    if (VERBOSE == 2)
        cout << "Initializing buffer index with: " << number << endl;
    // Initialize size
    partion_buffer_index = new atomic<int>[number];
    for (int i = 0; i < number; i++) {
        // Print debug info and index
        if (VERBOSE == 2)
            cout << "Partition: " << i << " Index: " << partion_buffer_index[i].load() << endl;
        partion_buffer_index[i].store(0, std::memory_order_relaxed);
    }
}

void AtomicMethod::work(int thread_index, const vector<tuple<uint64_t, uint64_t>>& data, int start_index, int bucket_size) {
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

void AtomicMethod::thread_work(const vector<tuple<uint64_t, uint64_t>>& data) {
    // Initialize mutexes for each partition
    init_mutexes();
    init_buffers();
    cout << "init_buffer_index start" << endl;
    init_buffer_index();
    cout << "init_buffer_index end" << endl;

    // Sleep here for 1 sec
    this_thread::sleep_for(chrono::seconds(1));

    cout << "Now we are print buffer indexes with partion size " << get_num_partitions() << endl;
    for(int i = 0; i < get_num_partitions(); i++) {
        int a_int = partion_buffer_index[i].load();
        cout << "Partition: " << i << " size: " <<  a_int << endl;
    }

    cout << "Now ended print of partion size " << endl;

    // Create buffers for each partition
    auto bucket_size = data.size() / NUM_THREADS;
    if (VERBOSE == 2)
        cout << "Starting with " << NUM_THREADS << " threads and bucket size " << bucket_size << endl;

    // Initialize threads
    
    cout << "Starting with " << NUM_THREADS << " threads and bucket size " << bucket_size << endl;
    vector<thread> threads(NUM_THREADS);
    for (int i = 0; i < NUM_THREADS; ++i) {
        threads[i] = thread(&AtomicMethod::work, this, i, cref(data), i * bucket_size, bucket_size);
    }

    cout << "Thread initialized " << bucket_size << endl;
    
    // Join threads
    for (auto& t : threads) {
        t.join();
    }
}

void AtomicMethod::add_tuple_to_buffer(int partition_key, tuple<uint64_t, uint64_t> tuple_to_add) {

    int old_index;
    int new_index;
    do 
    {
        // Get the current index in partition
        old_index = partion_buffer_index[partition_key].load(std::memory_order_relaxed);
        // Get the new value
        new_index = old_index + 1;
        
    }  while (!partion_buffer_index[partition_key].compare_exchange_weak(old_index, new_index));
    
    // use old index
    cout << "Partition: " << partition_key << " Index: " << old_index << " Key: " << get<0>(tuple_to_add) << " Value: " << get<1>(tuple_to_add) << endl;
    concurrent_buffers[partition_key][old_index] = tuple_to_add;
}

void AtomicMethod::print_buffers_everything() {
    for (int i = 0; i < concurrent_buffers.size(); i++) {
        for (int j = 0; j < concurrent_buffers[i].size(); j++) {
            cout << "Partition: " << i << " Key: " << get<0>(concurrent_buffers[i][j])
                << " Value: " << get<1>(concurrent_buffers[i][j]) << endl;
        }
    }
}

void AtomicMethod::print_buffers_partition_entries() {
    for (int i = 0; i < concurrent_buffers.size(); i++) {
        auto partition_size = concurrent_buffers[i].size();
        // counter_arr[i] = partition_size;
        cout << "Partition: " << i << "# entries: " << partition_size << endl;
    }
}

void AtomicMethod::print_buffers_partition_statistics() {
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
