// adj_matrix_graph.cpp
#include "graph.hpp"
#include <iostream>
#include <vector>
#include <stack>

class AdjMatrixGraph : public Graph {
    int vertex;
    std::vector<std::vector<bool>> adj;

public:
    AdjMatrixGraph(int vertex) : vertex(vertex), adj(vertex + 1, std::vector<bool>(vertex + 1, false)) {}

    void addEdge(int u, int v) override {
        adj[u][v] = true;
    }

    void runKosaraju() override {
        // Kosaraju algorithm implementation with an adjacency matrix.
        std::cout << "Kosaraju algorithm on adjacency matrix not fully implemented here." << std::endl;
    }
};
