#include "graph.hpp"
#include "deque_graph.cpp"  
#include <iostream>
#include <sstream>
#include <string>

typedef DequeGraph SelectedGraph;

int main() {
    std::string line;
    Graph* graph = nullptr;

    while (std::getline(std::cin, line)) {
        std::istringstream input(line);
        std::string command;
        input >> command;

        // Create Newgraph
        if (command == "Newgraph") {
            int n, m;
            input >> n >> m;
            // Clean up any existing graph
            delete graph;
            graph = new SelectedGraph(n);

            // Read m edges and add them to the graph
            for (int i = 0; i < m; ++i) {
                int u, v;
                // Read next edge
                std::getline(std::cin, line); 
                std::istringstream edgeInput(line);
                edgeInput >> u >> v;
                graph->addEdge(u, v);
            }
            std::cout << "Graph created with " << n << " vertices and " << m << " edges." << std::endl;

        // Do Kosaraju
        } else if (command == "Kosaraju") {
            if (graph) {
                graph->runKosaraju();
            } else {
                std::cerr << "No graph available. Use Newgraph command first." << std::endl;
            }

        // Add New edge
        } else if (command == "Newedge") {
            int u, v;
            input >> u >> v;
            if (graph) {
                graph->addEdge(u, v);
                std::cout << "Edge added from " << u << " to " << v << "." << std::endl;
            } else {
                std::cerr << "No graph available. Use Newgraph command first." << std::endl;
            }

        // Remove edge
        } else if (command == "Removeedge") {
            int u, v;
            input >> u >> v;
            if (graph) {
                graph->removeEdge(u, v);
                std::cout << "Edge removed from " << u << " to " << v << "." << std::endl;
            } else {
                std::cerr << "No graph available. Use Newgraph command first." << std::endl;
            }
        } else if (command == "***") {
            break;
        } else {
            std::cerr << "Unknown command: " << command << std::endl;
        }
    }

    delete graph;
    return 0;
}
