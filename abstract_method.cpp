#include "abstract_method.h"
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

AbstractMethod::AbstractMethod(ProgramArgs &args) {
    this->HASH_BITS = args.hash_bits;
    this->NUM_THREADS = args.num_threads;
    this->DATA_SIZE = args.data_size;
    this->VERBOSE = args.verbose;
    this->args = &args;
}

int AbstractMethod::get_num_partitions() const {
    return 1 << HASH_BITS;
}

int AbstractMethod::hash_function(uint64_t key) const {
    return key % (1 << HASH_BITS);
}

void AbstractMethod::print_hash_values(const vector<tuple<uint64_t, uint64_t>>& data) const {
    for (const tuple<uint64_t, uint64_t>& entry : data) {
        cout << hash_function(get<0>(entry)) << " " << get<1>(entry) << endl;
    }
}

bool AbstractMethod::read_affinity_file() {

    if(args->affinity_file.empty() || args->affinity_file == "none"){
        cout << "No affinity file specified" << endl;
        return false;
    }

    // Read line from file
    ifstream file;
    file.open(args->affinity_file);
    string line;
    getline(file, line);
    file.close();

    // Convert to vector<int>
    stringstream ss(line);
    string word;
    while(ss >> word) {
        affinity.push_back(stoi(word));
    }

    return true;
}