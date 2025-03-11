#include "independent_method.h"
#include <cmath>

using namespace std;

void IndependentMethod::work(int thread_index, const vector<tuple<uint64_t, uint64_t>>& data, int start_index, int bucket_size) {
    vector<vector<tuple<uint64_t, uint64_t>>> buffer(get_num_partitions());

    if (VERBOSE == 2) {
        cerr << "Thread #" << thread_index << ": start_index= " << start_index << endl;
    }
    // Identify the partition by hash function
    for (int i = start_index; i < start_index + bucket_size; i++) {
        // Hash key to get the partition key
        uint64_t key = get<0>(data[i]);
        int partition_key = hash_function(key);

        // Add tuple to local buffer
        buffer[partition_key].push_back(data[i]);
    }
    buffer_collection[thread_index] = move(buffer);

    if (VERBOSE == 2) {
        cerr << "Thread #" << thread_index << " completed... " << endl;
    }
}

void IndependentMethod::thread_work_affinity(const vector<tuple<uint64_t, uint64_t>>& data){
    // Temporary solution to avoid unused parameter warning
    
    buffer_collection.resize(NUM_THREADS);
    uint64_t bucket_size = data.size() / NUM_THREADS;

    if (VERBOSE == 2)
        cerr << "Starting with " << NUM_THREADS << " threads and bucket size " << bucket_size << endl;

    
    auto is_affinity_valid = read_affinity_file();

    if (!is_affinity_valid) {
        cerr << "Affinity file is not valid" << endl;
        exit(1);
    }

    vector<thread> threads(NUM_THREADS);
    for (int i = 0; i < NUM_THREADS; ++i) {
        threads[i] = thread(&IndependentMethod::work, this, i, cref(data), i * bucket_size, bucket_size);

        cpu_set_t cpuset;
        CPU_ZERO(&cpuset);
        CPU_SET(affinity[i], &cpuset);
        int rc = pthread_setaffinity_np(threads[i].native_handle(), sizeof(cpu_set_t), &cpuset);
        
        if (VERBOSE == 2)
            cerr << "Thread #" << i << " @ " << affinity[i] <<  "- start_index: " << i * bucket_size << endl;
        if (rc != 0) {
            cerr << "Error calling pthread_setaffinity_np: " << rc << endl;
        }

    }

    for (thread& t : threads) {
        t.join();
    }


}
void IndependentMethod::thread_work(const vector<tuple<uint64_t, uint64_t>>& data) {
    buffer_collection.resize(NUM_THREADS);
    uint64_t bucket_size = data.size() / NUM_THREADS;

    if (VERBOSE == 2) {
        cerr << "Starting with " << NUM_THREADS << " threads and bucket size " << bucket_size << endl;
    }

    vector<thread> threads(NUM_THREADS);
    for (int i = 0; i < NUM_THREADS; ++i) {
        threads[i] = thread(&IndependentMethod::work, this, i, cref(data), i * bucket_size, bucket_size);
    }

    for (thread& t : threads) {
        t.join();
    }
}

void IndependentMethod::print_buffers_everything() {
    for (int i = 0; i < buffer_collection.size(); i++) {
        for (int j = 0; j < buffer_collection[i].size(); j++) {
            for (int k = 0; k < buffer_collection[i][j].size(); k++) {
                cerr << "Thread: " << i << " Partition: " << j
                    << " Key: " << get<0>(buffer_collection[i][j][k])
                    << " Value: " << get<1>(buffer_collection[i][j][k]) << endl;
            }
        }
    }
}

void IndependentMethod::print_buffers_partition_entries() {
    for (int i = 0; i < buffer_collection.size(); i++) {
        for (int j = 0; j < buffer_collection[i].size(); j++) {
            int partition_size = buffer_collection[i][j].size();
            cerr << "Thread: " << i << " Partition: " << j << "# entries: " << partition_size << endl;
        }
    }
}

void IndependentMethod::print_buffers_partition_statistics() {
    int num_partitions = get_num_partitions();

    float mean = (DATA_SIZE / num_partitions) / NUM_THREADS;
    float std_dev = 0.0;

    for (int i = 0; i < buffer_collection.size(); i++) {
        for (int j = 0; j < buffer_collection[i].size(); j++) {
            int partition_size = buffer_collection[i][j].size();
            float variance = partition_size - mean;
            std_dev += variance * variance;
        }
    }

    std_dev = sqrt(std_dev / num_partitions);
    if (VERBOSE == 1) {
        cerr << "Expected Partition Size: " << mean << " ±" << std_dev << endl;
    }
}
