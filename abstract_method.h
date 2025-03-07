#ifndef ABSTRACT_METHOD_H
#define ABSTRACT_METHOD_H

#include <iostream>
#include <tuple>
#include <vector>
#include <thread>
#include <cstdint>

using namespace std;

class AbstractMethod {
public:
    int HASH_BITS;
    int NUM_THREADS;
    int DATA_SIZE;
    int VERBOSE;
    
    AbstractMethod(int hash_bits, int num_threads, int data_size, int verbose);
    virtual ~AbstractMethod() = default;

    int get_num_partitions() const;
    int hash_function(uint64_t key) const;
    void print_hash_values(const vector<tuple<uint64_t, uint64_t>>& data) const;

    virtual void work(int thread_index, const vector<tuple<uint64_t, uint64_t>>& data, int start_index, int bucket_size) = 0;
    virtual void thread_work(const vector<tuple<uint64_t, uint64_t>>& data) = 0;
    virtual void print_buffers_everything() = 0;
    virtual void print_buffers_partition_entries() = 0;
    virtual void print_buffers_partition_statistics() = 0;
};

#endif
