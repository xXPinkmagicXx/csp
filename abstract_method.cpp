#include "abstract_method.h"

using namespace std;

AbstractMethod::AbstractMethod(int hash_bits, int num_threads, int data_size, int verbose) {
    this->HASH_BITS = hash_bits;
    this->NUM_THREADS = num_threads;
    this->DATA_SIZE = data_size;
    this->VERBOSE = verbose;
}

int AbstractMethod::get_num_partitions() const {
    return 1 << HASH_BITS;
}

int AbstractMethod::hash_function(uint64_t key) const {
    return key % (1 << HASH_BITS);
}

void AbstractMethod::print_hash_values(const vector<tuple<uint64_t, uint64_t>>& data) const {
    for (const auto& entry : data) {
        cout << hash_function(get<0>(entry)) << " " << get<1>(entry) << endl;
    }
}
