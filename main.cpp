#include <iostream>
#include <random>
#include <tuple>
#include <fstream>
#include <vector>

using namespace std;

const string fileName = "random_integers.txt";

// Cast one string line to a integer  tupleto  
tuple<uint64_t, uint64_t> cast_to_tuple(std::string line) {
    size_t pos = line.find(' ');
    
    uint64_t first = stoull(line.substr(0, pos));
    uint64_t second = stoull(line.substr(pos + 1));

    tuple<uint64_t, uint64_t> result(first, second);
    
    return result;
}

vector<tuple<uint64_t, uint64_t>> read_file(){
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

int main(int argc, char *argv[]){

    // Read generated file with random tuples of usigned 64 bit integers
    auto data = read_file();    
    // for(auto tuple : data){
    //     cout << get<0>(tuple) << " " << get<1>(tuple) << endl;
    // }

    return 0;
}

