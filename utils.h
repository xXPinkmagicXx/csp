#ifndef UTILS_H
#define UTILS_H

#include <iostream>

using namespace std;

struct ProgramArgs {
    int hash_bits = 4;
    int num_threads = 4 ;
    int verbose = 0;
    int method_type = 0;
    int data_size = 0;
    string method_name = "";
    string affinity_file = "";
    string output_file_name = "";
};

#endif
