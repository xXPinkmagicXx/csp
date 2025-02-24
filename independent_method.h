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

        void work(int thread_index, const vector<tuple<uint64_t, uint64_t>>& data, int start_index, int bucket_size) {
            vector<vector<tuple<uint64_t, uint64_t>>> buffer(get_num_partitions());
            if(VERBOSE == 2)
                cout << "Thread #" << thread_index << ": start_index= " << start_index << endl;
            // Identify the partition by hash function
            for(int i = start_index; i < start_index + bucket_size; i++) {
                // Hash key to get the partition key
                auto key = get<0>(data[i]);
                auto partition_key = hash_function(key);
                
                // Add tuple to local buffer
                buffer[partition_key].push_back(data[i]);
            }
            buffer_collection[thread_index] = move(buffer);
            if(VERBOSE == 2)
                cout << "Thread #" << thread_index << " completed... "<< endl;
        }      

        void thread_work(const vector<tuple<uint64_t, uint64_t>>& data) {
            buffer_collection.resize(NUM_THREADS);
            // Create buffers for each partition
            auto bucket_size = data.size() / NUM_THREADS;
            if(VERBOSE == 2)
                cout << "Starting with " << NUM_THREADS << " threads and bucket size " << bucket_size << endl;
            
            // Initialize threads
            vector<thread> threads(NUM_THREADS);
            for (int i = 0; i < NUM_THREADS; ++i) {
              threads[i] = thread(&IndependentMethod::work, this, i, data, i * bucket_size, bucket_size);

            }
            // Join threads
            for (auto& t : threads) {
              t.join();
            }
        }

        void print_buffers_everything() {
            for (int i = 0; i < buffer_collection.size(); i ++) {
                for(int j = 0; j < buffer_collection[i].size(); j++){
                    for(int k = 0; k < buffer_collection[i][j].size(); k++){
                        cout << "Thread: " << i << "Partition: " << j << " Key: " << get<0>(buffer_collection[i][j][k]) << " Value: " << get<1>(buffer_collection[i][j][k]) << endl;
                    }
                }
            }
        }
        
        void print_buffers_partition_entries() {
            for(int i = 0; i < buffer_collection.size(); i++) {
                for(int j = 0; j < buffer_collection[i].size(); j++){
                    auto partition_size = buffer_collection[i][j].size();
                    // counter_arr[i] = partition_size;
                    cout << "Thread: " << i << "Partition: " << j << "# entries: " << partition_size << endl;
                }
            }
        }

        void print_buffers_partition_statistics() {
            cout << "TODO";
        }

        /*
        void print_buffers_partition_statistics() {
            int num_partitions = get_num_partitions();
            auto counter_arr = new int[num_partitions];
            
            float mean = DATA_SIZE / num_partitions;
            float std_dev = 0.0;
        
            for(int i = 0; i < concurrent_buffers.size(); i++){
                auto partition_size = concurrent_buffers[i].size();
                // counter_arr[i] = partition_size;
                auto variance = partition_size - mean;
                std_dev += variance * variance; 
            }
        
            std_dev = sqrt(std_dev / num_partitions);
            cout << "Expected Partion Size: " << mean << " ±" << std_dev << endl;
        }*/
 };

 #endif