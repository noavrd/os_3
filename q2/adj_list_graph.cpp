// adj_list_graph.cpp
#include "graph.hpp"
#include <iostream>
#include <vector>
#include <list>
#include <stack>

class AdjListGraph : public Graph {
    int V;
    std::vector<std::list<int>> adj;

public:
    AdjListGraph(int V) : V(V), adj(V + 1) {}

    void addEdge(int u, int v) override {
        adj[u].push_back(v);
    }

    void runKosaraju() override {
        // Kosaraju algorithm implementation with an adjacency list
        std::cout << "Kosaraju algorithm on adjacency list not fully implemented here." << std::endl;
    }
};
