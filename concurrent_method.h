#ifndef CONCURRENT_METHOD_H
#define CONCURRENT_METHOD_H

#include "abstract_method.h"
#include <iostream>
#include <random>
#include <tuple>
#include <vector>
#include <thread>
#include <chrono>
#include <mutex>

using namespace std;

class ConcurrentMethod : public AbstractMethod {
public:
    mutex *mutexes;
    vector<vector<tuple<uint64_t, uint64_t>>> concurrent_buffers; // one buffer per partition

    using AbstractMethod::AbstractMethod;

    void init_mutexes();
    void init_buffers();
    void work(int thread_index, const vector<tuple<uint64_t, uint64_t>>& data, int start_index, int bucket_size);
    void thread_work(const vector<tuple<uint64_t, uint64_t>>& data);
    void thread_work_affinity(const vector<tuple<uint64_t, uint64_t>>& data);
    void add_tuple_to_buffer(int partition_key, tuple<uint64_t, uint64_t> tuple_to_add);
    void print_buffers_everything();
    void print_buffers_partition_entries();
    void print_buffers_partition_statistics();
};

#endif
