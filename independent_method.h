#ifndef INDEPENDENT_METHOD_H
#define INDEPENDENT_METHOD_H

#include "abstract_method.h"
#include <iostream>
#include <random>
#include <tuple>
#include <vector>
#include <thread>
#include <chrono>
#include <mutex>

using namespace std;

class IndependentMethod : public AbstractMethod {
public:
    vector<vector<vector<tuple<uint64_t, uint64_t>>>> buffer_collection;

    using AbstractMethod::AbstractMethod;

    void work(int thread_index, const vector<tuple<uint64_t, uint64_t>>& data, int start_index, int bucket_size);
    double thread_work(const vector<tuple<uint64_t, uint64_t>>& data);
    double thread_work_affinity(const vector<tuple<uint64_t, uint64_t>>& data);
    void print_buffers_everything();
    void print_buffers_partition_entries();
    void print_buffers_partition_statistics();
};

#endif
