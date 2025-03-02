#ifndef ATOMIC_METHOD_H
#define ATOMIC_METHOD_H

#include "abstract_method.h"
#include <iostream>
#include <random>
#include <tuple>
#include <vector>
#include <thread>
#include <chrono>
#include <mutex>

using namespace std;

class AtomicMethod : public AbstractMethod {
public:
    
    mutex *mutexes;
    vector<vector<tuple<uint64_t, uint64_t>>> concurrent_buffers; // one buffer per partition
    std::array<std::array<tuple<uint64_t, uint64_t>>> atomic_buffers; // one buffer per partition
    atomic<int> partion_buffer_index;

    using AbstractMethod::AbstractMethod;

    void init_mutexes();
    void init_buffers();
    void init_buffer_index();
    void work(int thread_index, const vector<tuple<uint64_t, uint64_t>>& data, int start_index, int bucket_size);
    void thread_work(const vector<tuple<uint64_t, uint64_t>>& data);
    void add_tuple_to_buffer(int partition_key, tuple<uint64_t, uint64_t> tuple_to_add);
    void add_tuple_to_buffer_atomic(int partition_key, tuple<uint64_t, uint64_t> tuple_to_add);
    void print_buffers_everything();
    void print_buffers_partition_entries();
    void print_buffers_partition_statistics();
};

#endif
