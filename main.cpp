#include <iostream>
#include <random>
#include <tuple>
#include <fstream>

using namespace std;


// Method for generating a tuple of integers of 64 bits (only positive)
tuple<uint64_t, uint64_t> generate_random_integers() {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<uint64_t> dis(std::numeric_limits<uint64_t>::min(), std::numeric_limits<uint64_t>::max());
    tuple<uint64_t, uint64_t> result(dis(gen), dis(gen));
    return result;
}

void genereate_file(int num_to_generate){
    ofstream file;
    file.open("random_integers.txt", ios::trunc);
    for(int i = 0; i < num_to_generate; i++){
        auto result = generate_random_integers();
        file << get<0>(result) << " " << get<1>(result) << endl;
    }
    file.close();
}

int main(int argc, char *argv[]){

    if(argc <= 1){
        cout << "Usage: " << argv[0] << endl;
        return 1;
    }

    // Generate this many number of uniformly random tuples
    int generate_num = stoi(argv[1]);
    
    if(generate_num <= 0){
        cout << "Please enter a valid number" << endl;
        return 1;
    }
    if (generate_num > 2^24+1){
        cout << "Please enter a number less than 2^24" << endl;
        return 1;
    }

    // Generate file with random tuples of usigned 64 bit integers
    genereate_file(generate_num);
    



    auto result = generate_random_integers();
    std::cout << "This is a tuple " << get<1>(result) << std::endl;
    return 0;
}

