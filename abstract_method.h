#ifndef ABSTRACT_METHOD_H
#define ABSTRACT_METHOD_H

#include <iostream>
#include <tuple>
#include <vector>
#include <thread>

using namespace std;

class AbstractMethod {
    public:
        int HASH_BITS;
        int NUM_THREADS;
        int DATA_SIZE = 0;
        int VERBOSE = 0;

        AbstractMethod(int hash_bits, int num_threads, int data_size, int verbose) {
            HASH_BITS = hash_bits;
            NUM_THREADS = num_threads;
            DATA_SIZE = data_size;
            VERBOSE = verbose;
          }

        int get_num_partitions() {
            return 1 << HASH_BITS;
        }

        int hash_function(uint64_t key) {
            return key % (1 << HASH_BITS);
        }

        void print_hash_values(vector<tuple<uint64_t, uint64_t>> data) {
            for(int i = 0; i < data.size(); i++){
                cout << hash_function(get<0>(data[i])) << " " << get<1>(data[i]) << endl;
            }
        }

        virtual ~AbstractMethod() = default;

        virtual void work(int thread_index, const vector<tuple<uint64_t, uint64_t>>& data, int start_index, int bucket_size) = 0;
       
        virtual void thread_work(const vector<tuple<uint64_t, uint64_t>>& data) = 0;

        virtual void print_buffers_everything() = 0;

        virtual void print_buffers_partition_entries() = 0;

        virtual void print_buffers_partition_statistics() = 0;
 };

 #endif