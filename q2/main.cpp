// main.cpp
#include "graph.hpp"
#include <iostream>

#ifdef USE_DEQUE
#include "deque_graph.cpp"
typedef DequeGraph SelectedGraph;
#elif defined(USE_LIST)
#include "list_graph.cpp"
typedef ListGraph SelectedGraph;
#endif

int main() {
    int n, m;
    std::cin >> n >> m;

    Graph* graph = new SelectedGraph(n);

    for (int i = 0; i < m; i++) {
        int u, v;
        std::cin >> u >> v;
        graph->addEdge(u, v);
    }

    graph->runKosaraju();
    delete graph;

    return 0;
}
