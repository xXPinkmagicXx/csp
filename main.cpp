#include "concurrent_method.h"
#include "independent_method.h"
#include "abstract_method.h"
#include "utils.h"
#include <tuple>
#include <fstream>
#include <vector>
#include <thread>
// #include <pthread>
#include <chrono>

using namespace std;

// Global variables
ProgramArgs args;
const string input_file = "random_integers.txt";
const string output_dir = "./results/";
const string output_file_extension = ".csv";

// Cast one string line to a unsigned 64-bit integer tuplet  
tuple<uint64_t, uint64_t> cast_to_tuple(string line) {
    size_t pos = line.find(' ');
    
    uint64_t first = stoull(line.substr(0, pos));
    uint64_t second = stoull(line.substr(pos + 1));

    tuple<uint64_t, uint64_t> result(first, second);
    
    return result;
}

// Read generated file with random tuples of unsigned 64-bit integers
vector<tuple<uint64_t, uint64_t>> read_file() {
    ifstream file(input_file);
    string str;
    vector<tuple<uint64_t, uint64_t>> tuples;
    while (getline(file, str))
    {
        tuple<uint64_t, uint64_t> tuple = cast_to_tuple(str);
        tuples.push_back(tuple);
    }
    return tuples;
}

bool is_power_of_two(uint64_t x) {
    return (x & (x - 1)) == 0;
}

void write_results_to_file(string path, float million_tuples_per_second) {
    //ofstream file;
    //file.open(path, ios::app);
    // Write ulong to file
    cout << args.hash_bits << "," << million_tuples_per_second << endl;
    //file.close();
}

void do_method(AbstractMethod& method, const vector<tuple<uint64_t, uint64_t>>& data) {

    int duration = method.thread_work_affinity(cref(data));
    
    if(args.verbose == 2){
        cout << "NEW Time taken: " << duration << " milliseconds" << endl;
    }

    // print summary
    float tuples_pr_ms = args.data_size / duration;
    float tuples_per_second = tuples_pr_ms * 1000;
    float million_tuples_per_second = tuples_per_second / 1000000;
    write_results_to_file(output_dir + args.output_file_name + output_file_extension, million_tuples_per_second);
    if(args.verbose == 1) {
        method.print_buffers_partition_statistics();
        // cerr << "Time taken: " << duration << " milliseconds" << endl;
        cerr << "Million Tuples per second: " << million_tuples_per_second << endl; 
    }
}

bool read_args(int argc, char *argv[], ProgramArgs &args) {
    try {
        if (argc > 1) {
            // Get and validate the hash bits argument 
            args.hash_bits = stoi(argv[1]);
            // cerr << "zero arg: " << argv[0] << endl;
            // cerr << "first arg: " << argv[1] << endl;
            if (args.hash_bits <= 0 || args.hash_bits > 18) {
                cerr << "First arg: enter a positive number between 1-18" << endl;
                return false;
            }
        }

        if (argc > 2) {
            // Get and validate the # threads argument
            args.num_threads = stoi(argv[2]);
            // cerr << "second arg: " << argv[2] << endl;
            if (args.num_threads <= 0 || args.num_threads > 32) {
                cerr << "Second arg: enter positive number between 1-32" << endl;
                return false;
            }
        }

        if (argc > 3) {
            // Get and validate the verbose argument
            args.verbose = stoi(argv[3]);
            // cerr << "third arg: " << argv[3] << endl;
            if (args.verbose < 0 || args.verbose >= 3) {
                cerr << "Third arg: enter a number between 0-2 for verbosity" << endl;
                return false;
            }
        }

        if (argc > 4) {
            // Get and validate the method type argument
            // cerr << "Forth arg: " << argv[4] << endl;
            args.method_type = stoi(argv[4]);
        }

        if (argc > 5) {
            // Optional affinity file
            // cerr << "fifth arg: " << argv[5] << endl;
            args.affinity_name = argv[5];
        }
    } catch (const invalid_argument& e) {
        cerr << "Invalid argument: " << e.what() << endl;
        return false;
    } catch (const out_of_range& e) {
        cerr << "Argument out of range: " << e.what() << endl;
        return false;
    }
    
    args.method_name = args.method_type == 0  ? "concurrent" : "independent";
    args.affinity_file = "affinity/" + args.affinity_name + ".txt";
    args.output_file_name =  args.method_name + "_" + args.affinity_name + "_" + to_string(args.num_threads);

    return true;
}

bool validate_input(const vector<tuple<uint64_t, uint64_t>>& data){
    uint64_t data_size = data.size();
    if(args.verbose == 2) {
        cerr << "Data size: " << data_size << endl;
    }
    if(data_size == 0) {
        cerr << "No data to process" << endl;
        cerr << "Closing..." << endl;
        return false;
    } 
    
    if(data_size < args.num_threads) {
        cerr << "Data size is less than the number of threads"  << endl;
        cerr << "Data size:" << data_size << "# threads" << args.num_threads << endl;
        cerr << "Closing..." << endl;
        return false;
    }

    if(!is_power_of_two(data_size)) {
        cerr << "Data size is not a power of 2" << endl;
        cerr << "Use generate.o to generate data" << endl;
        cerr << "Closing..." << endl;
        return false;
    }

    return true;
}

int main(int argc, char *argv[]) {
    auto was_successful = read_args(argc, argv, args);
    if(!was_successful) {
        cerr << "Reading args failed..." << endl;
        return 1;
    }
    
    // Read data from file
    // Note that the data is/should be read only
    vector<tuple<uint64_t, uint64_t>> data = read_file();    
    // Validate input
    auto is_data_valid = validate_input(cref(data));
    if(!is_data_valid) {
        cerr << "Data is not valid..." << endl;
        return 1;
    }
    
    args.data_size = data.size();
    
    
    // Do the correct type of partitioning
    if (args.method_type == 0) {
        IndependentMethod method(args);
        do_method(method, cref(data));

    } else if (args.method_type == 1) {
        ConcurrentMethod method(args);
        do_method(method, cref(data));

    } else {
        cerr << "Unknown method type" << endl;
        cerr << "Closing..." << endl;
        return 1;
    }

    return 0;
}