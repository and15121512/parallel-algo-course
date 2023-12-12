#include <iostream>
#include <tuple>
#include <chrono>
#include <cmath>
#include <parlay/parallel.h>
#include <parlay/sequence.h>

struct Vertex {
    int x, y, z;
    
    Vertex() {}
    
    Vertex(int x, int y, int z) : x(x), y(y), z(z) {}
};

int reindex(int x, int y, int z, int N) {
    return N * N * x + N * y + z;
}

parlay::sequence<parlay::sequence<int>> generate_cubic_graph(int num_vertices) {
    Vertex* vertices = new Vertex[num_vertices * num_vertices * num_vertices];
    int vertex_count = 0;
    
    for (int x = 0; x < num_vertices; x++) {
        for (int y = 0; y < num_vertices; y++) {
            for (int z = 0; z < num_vertices; z++) {
                vertices[vertex_count++] = Vertex(x, y, z);
            }
        }
    }

    int max_edges = 6 * vertex_count; // Maximum number of edges
    int** edges = new int*[max_edges]; // Array of edges
    int* edge_count = new int[vertex_count]; // Number of edges for each vertex

    for (int i = 0; i < vertex_count; i++) {
        edge_count[i] = 0;
    }

    auto add_edge = [&](int u, int v) {
        edges[u][edge_count[u]++] = v;
    };

    for (int i = 0; i < max_edges; i++) {
        edges[i] = new int[2];
    }

    for (int i = 0; i < vertex_count; i++) {
        int x = vertices[i].x;
        int y = vertices[i].y;
        int z = vertices[i].z;
        // Add edges to the neighboring vertices in all six directions
        for (const auto& direction : std::vector<std::tuple<int, int, int>>{{1, 0, 0}, {-1, 0, 0}, {0, 1, 0},
                                                                           {0, -1, 0}, {0, 0, 1}, {0, 0, -1}}) {
            int dx = std::get<0>(direction);
            int dy = std::get<1>(direction);
            int dz = std::get<2>(direction);
            int neighbor_x = x + dx;
            int neighbor_y = y + dy;
            int neighbor_z = z + dz;
            if (neighbor_x >= 0 && neighbor_x < num_vertices &&
                neighbor_y >= 0 && neighbor_y < num_vertices &&
                neighbor_z >= 0 && neighbor_z < num_vertices) {
                int u = reindex(x, y, z, num_vertices);
                int v = reindex(neighbor_x, neighbor_y, neighbor_z, num_vertices);
                add_edge(u, v);
            }
        }
    }

    parlay::sequence<parlay::sequence<int>> adjacency_lists(num_vertices * num_vertices * num_vertices);
    for (int i = 0; i < vertex_count; i++) {
        int u = reindex(vertices[i].x, vertices[i].y, vertices[i].z, num_vertices);
        for (int j = 0; j < edge_count[i]; j++) {
            int v = edges[i][j];
            adjacency_lists[u].push_back(v);
        }
    }

    // Clean up memory
    for (int i = 0; i < max_edges; i++) {
        delete[] edges[i];
    }
    delete[] edges;
    delete[] edge_count;
    delete[] vertices;

    return adjacency_lists;
}

//int main() {
//    int num_vertices = 3;  // Change this to the desired number of vertices
//
//    auto start_time = std::chrono::high_resolution_clock::now();
//    parlay::sequence<parlay::sequence<int>> cubic_graph = generate_cubic_graph(num_vertices);
//    auto end_time = std::chrono::high_resolution_clock::now();
//    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
//
//    // Print the adjacency lists
//    for (int i = 0; i < cubic_graph.size(); i++) {
//        std::cout << "Vertex " << i << ": ";
//        std::sort(cubic_graph[i].begin(), cubic_graph[i].end()); // Sort the neighbor vertices
//        cubic_graph[i].erase(std::unique(cubic_graph[i].begin(), cubic_graph[i].end()), cubic_graph[i].end()); // Remove duplicates
//        for (const auto& neighbor : cubic_graph[i]) {
//            std::cout << neighbor << " ";
//        }
//        std::cout << std::endl;
//    }
//
//    std::cout << "Execution Time: " << std::round(duration.count()/1e3)/1e3 << " s " << std::endl;
//
//    return 0;
//}
