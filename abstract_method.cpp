#include "abstract_method.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <thread>

//may return 0 when not able to detect

using namespace std;

AbstractMethod::AbstractMethod(ProgramArgs &args) {
    this->HASH_BITS = args.hash_bits;
    this->NUM_THREADS = args.num_threads;
    this->DATA_SIZE = args.data_size;
    this->VERBOSE = args.verbose;
    this->args = &args;
    this->processor_count = std::thread::hardware_concurrency();

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
    if(args->verbose == 2){
        cout << processor_count << " processors detected" << endl;
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
        
        try {
            int core = stoi(word);
            if(core >= processor_count) {
                throw out_of_range("Core number out of range");
            }
            affinity.push_back(core);
        } catch (const invalid_argument& e) {
            cout << "Invalid core number: " << word << endl;
            return false;
        } catch (const out_of_range& e) {
            cout << "Core number out of range: " << word << endl;
            return false;
        }

    }
    
    // Print That we are populating affinity vector
    if(affinity.size() < NUM_THREADS && args->verbose == 2) {
        cout << "Affinity size:" << to_string(affinity.size()) << " < threads: " << to_string(args->num_threads) << endl;
        cout << "Populating affinity vector:" << endl;
    }

    int current_index = 0;
    while (affinity.size() < NUM_THREADS) {
        affinity.push_back(affinity[current_index % NUM_THREADS]);
    }

    return true;
}