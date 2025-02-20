#include <iostream>
#include <random>
#include <tuple>
#include <fstream>
#include <vector>

using namespace std;

const string fileName = "random_integers.txt";
int hash_bits = 4;

// Cast one string line to a integer  tupleto  
tuple<uint64_t, uint64_t> cast_to_tuple(std::string line) {
    size_t pos = line.find(' ');
    
    uint64_t first = stoull(line.substr(0, pos));
    uint64_t second = stoull(line.substr(pos + 1));

    tuple<uint64_t, uint64_t> result(first, second);
    
    return result;
}

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
    return key % (1 << hash_bits);
}

int main(int argc, char *argv[]) {

    // Read generated file with random tuples of usigned 64 bit integers
    if(argc < 1) {
        cout << "Usage: " << argv[0] << " <number of hash bits>" << endl;
        return 1;
    }
    auto hash_bit_arg = stoi(argv[1]);
    if(hash_bit_arg <= 0 || hash_bit_arg > 18) {
        cout << "Please enter a positive number between 1-18" << endl;
        return 1;
    }

    hash_bits = hash_bit_arg;

    auto data = read_file();    
    // for(auto tuple : data){
    //     cout << get<0>(tuple) << " " << get<1>(tuple) << endl;
    // }
    for(int i = 0; i < data.size(); i++){
        cout << hash_function(get<0>(data[i])) << " " << get<1>(data[i]) << endl;
    }
    // auto first_key = get<0>(data[1]);
    // auto hashed_key = hash_function(first_key);
    // cout << "First key: " << first_key << " Hashed key: " << hashed_key << endl;

    return 0;
}

