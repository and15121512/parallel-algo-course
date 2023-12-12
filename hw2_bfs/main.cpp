#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "parlay/parallel.h"
#include "parlay/sequence.h"

#include "cubic_graph.cpp"
#include "bfs.cpp"

using namespace std;
using namespace parlay;

typedef sequence<int> vertex;
typedef sequence<vertex> graph;

graph read_graph_from_file(const string& filename) {
    ifstream file(filename);
    graph adjacency_list;

    if (file.is_open()) {
        string line;
        while (getline(file, line)) {
            int vertex1, vertex2;
            istringstream iss(line);
            if (iss >> vertex1 >> vertex2) {
                int max_vertex = max(vertex1, vertex2);
                if (max_vertex >= adjacency_list.size()) {
                    adjacency_list.resize(max_vertex + 1);
                }
                adjacency_list[vertex1].push_back(vertex2);
            }
        }
        file.close();
    } else {
        cerr << "Unable to open file: " << filename << endl;
    }

    return adjacency_list;
}

void write_2d_array_to_file(const sequence<sequence<int>>& arr, const string& filename) {
    const int k_max_output_rows = 200;

    ofstream file(filename);

    if (file.is_open()) {
        int count = 0; // keep track of the number of sequences written
        for (const auto& row : arr) {
            if (count == k_max_output_rows) {
                break; // stop writing after N sequences
            }
            for (const auto& value : row) {
                file << value << " ";
            }
            file << endl;
            count++;
        }
        file.close();
    } else {
        cerr << "Unable to open file: " << filename << endl;
    }
}

int main(int argc, char* argv[]) {
    string input_file = "cubic_graph.txt";
    string output_file = "frontiers.txt";

    if (argc < 3) {
        std::cout << std::string("Please provide 'method' and 'num_vertices' parameter. Values expected:")
                   + std::string("'method' in ['seq', 'par'], 'num_vertices' is interger greater than zero.") << std::endl;
        return 1;
    }
    std::string method = argv[1];
    int num_vertices = std::stoi(argv[2]);
    
    size_t workers_cnt = parlay::num_workers();
    if (method == "seq" && workers_cnt != 1) {
        std::cout << "Number of parlaylib workers set: "+std::to_string(workers_cnt)+". Expected (for method=seq): 1" << std::endl;
        return 1;
    }
    if (method == "par" && workers_cnt != 4) {
        std::cout << "Number of parlaylib workers set: "+std::to_string(workers_cnt)+". Expected (for method=par): 4" << std::endl;
        return 1;
    }

    auto gen_start_time = std::chrono::high_resolution_clock::now();
    graph G;
    if (num_vertices == 0) {
        G = read_graph_from_file(input_file);
    }
    else {
        G = generate_cubic_graph(num_vertices);
    }
    auto gen_end_time = std::chrono::high_resolution_clock::now();
    auto gen_duration = std::chrono::duration_cast<std::chrono::microseconds>(gen_end_time - gen_start_time);

    auto start_time = std::chrono::high_resolution_clock::now();
    auto frontiers = BFS(0, G);
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    
    write_2d_array_to_file(frontiers, output_file);

    std::cout << "Generating Time: " << std::round(gen_duration.count()/1e3)/1e3 << " s "
              << "Execution Time: " << std::round(duration.count()/1e3)/1e3 << " s " << std::endl;

    return 0;
}