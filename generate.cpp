#include <iostream>
#include <random>
#include <tuple>
#include <fstream>
#include <set>

using namespace std;

random_device rd;
mt19937 gen(rd());
uniform_int_distribution<uint64_t> dis(numeric_limits<uint64_t>::min(), numeric_limits<uint64_t>::max());

set<uint64_t> generated_numbers;

bool is_power_of_two(ulong x){
    return (x & (x - 1)) == 0;
}

// Method for generating a tuple of integers of 64 bits (only positive)
tuple<uint64_t, uint64_t> generate_random_integers() {
    uint64_t key;
    // If key already exists, generate a new one
    do {
        key = dis(gen);
    }
    // Generate a random key and add to set
    while(!generated_numbers.insert(key).second);
    
    // Make tuple and return
    tuple<uint64_t, uint64_t> result(key, dis(gen));
    return result;
}

void generate_file(int num_to_generate) {
    ofstream file;
    cout << "Generating " << num_to_generate << " random integers" << endl;
    cout << "Opening file... " << endl;
    file.open("random_integers.txt", ios::trunc);
    for(int i = 0; i < num_to_generate; i++){
        auto result = generate_random_integers();
        file << get<0>(result) << " " << get<1>(result) << endl;
    }
    file.close();
    cout << "Closed file... " << endl;
}

bool keys() {
    // Check if all keys are unique
    return generated_numbers.size() == pow(2.0, 24.0);
}

void generate_affinity_linear(int n_threads) {
    ofstream file;
    cout << "Generating " << n_threads << " random integers" << endl;
    cout << "Opening file... " << endl;
    file.open("affinity.txt", ios::trunc);
    for(int i = 0; i < n_threads; i++){
        file << i << endl;
    }
    file.close();
    cout << "Closed file... " << endl;
}

void generate_affinity_even(int n_threads) {
    ofstream file;
    cout << "Generating " << n_threads << " random integers" << endl;
    cout << "Opening file... " << endl;
    file.open("affinity.txt", ios::trunc);
    for(int i = 0; i < n_threads; i++){
        file << i << endl;
    }
    file.close();
    cout << "Closed file... " << endl;
}

void generate_affinity_even(int n_threads) {
    ofstream file;
    cout << "Generating " << n_threads << " random integers" << endl;
    cout << "Opening file... " << endl;
    file.open("affinity.txt", ios::trunc);
    for(int i = 0; i < n_threads; i++){
        file << i * 2 << endl;
    }
    file.close();
    cout << "Closed file... " << endl;
}


int main(int argc, char *argv[]) {

    if(argc <= 1) {
        cout << "Usage: " << argv[0] << " <number of tuples>" << endl;
        return 1;
    }

    // Generate this many number of uniformly random tuples
    int generate_num = stoi(argv[1]);

    if(generate_num <= 0) {
        cout << "Please enter a positive number" << endl;
        cout << "Closing..." << endl;
        return 1;
    }

    if (generate_num > pow(2.0, 24.0)) {
        cout << "Please enter a number less than 2^24" << endl;
        cout << "Closing..." << endl;
        return 1;
    }

    if (!is_power_of_two(generate_num)){
        cout << "Please enter a number that is a power of 2" << endl;
        cout << "Closing..." << endl;
        return 1;
    }

    // Generate file with random tuples of usigned 64 bit integers
    generate_file(generate_num);

    cout << "Generation complete..." << endl;
    return 0;
}



