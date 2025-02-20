#include <iostream>
#include <random>
#include <tuple>
#include <fstream>
#include <vector>
#include <thread>
#include <chrono>
#include <mutex>
#include <pthread.h>


using namespace std;

const string fileName = "random_integers.txt";
int HASH_BITS = 4;
int NUM_THREADS = 4;

// Cast one string line to a integer  tupleto  
tuple<uint64_t, uint64_t> cast_to_tuple(std::string line) {
    size_t pos = line.find(' ');
    
    uint64_t first = stoull(line.substr(0, pos));
    uint64_t second = stoull(line.substr(pos + 1));

    tuple<uint64_t, uint64_t> result(first, second);
    
    return result;
}

// Read generated file with random tuples of usigned 64 bit integers
vector<tuple<uint64_t, uint64_t>> read_file() {
    ifstream file(fileName);
    string str;
    vector<tuple<uint64_t, uint64_t>> tuples;
    while (getline(file, str))
    {
        // Process str
        auto tuple = cast_to_tuple(str);
        tuples.push_back(tuple);
    }
    return tuples;
}

int hash_function(uint64_t key) {
    // Implement a hash function
    return key % (1 << HASH_BITS);
}

void print_hash_values(vector<tuple<uint64_t, uint64_t>> data) {
    for(int i = 0; i < data.size(); i++){
        cout << hash_function(get<0>(data[i])) << " " << get<1>(data[i]) << endl;
    }
}
void work(int thread_index, vector<tuple<uint64_t, uint64_t>> data, int start_index, int bucket_size){
    std::this_thread::sleep_for(chrono::milliseconds(thread_index));
    cout << "Thread #" << thread_index << ": start_index= " << start_index << endl;
}

void thread_work(vector<tuple<uint64_t, uint64_t>> data) {
    
    auto bucket_size = data.size() / NUM_THREADS;

    std::vector<std::thread> threads(NUM_THREADS);
    for (int i = 0; i < NUM_THREADS; ++i) {
      threads[i] = thread(work, i, data, i * bucket_size, bucket_size);
    }
  
    for (auto& t : threads) {
      t.join();
    }
  }


int main(int argc, char *argv[]) {
    int hash_bit_arg;
    int thread_count_arg;
    if(argc <= 2 ) {
        cout << "Using default values hash_bits=" << HASH_BITS << " thread_count=" << NUM_THREADS << endl;
    } else {
        hash_bit_arg = stoi(argv[1]);
        thread_count_arg = stoi(argv[2]);
        
        if(hash_bit_arg <= 0 || hash_bit_arg > 18) {
            cout << "Please enter a positive number between 1-18" << endl;
            return 1;
        }
    
        if(thread_count_arg <= 0 || thread_count_arg > 32) {
            cout << "Please enter a positive number between 1-32" << endl;
            return 1;
        }
        
        HASH_BITS = hash_bit_arg;
        NUM_THREADS = thread_count_arg;
    }
    
    
    // Read data from file
    auto data = read_file();    

    // print_hash_values(data);

    // Do the work in threads
    thread_work(data);

    return 0;
}

