#include "Graph.hpp"
#include <iostream>

int main() {
    int n, m;
    std::cin >> n >> m;

    Graph g(n);

    for (int i = 0; i < m; i++) {
        int u, v;
        std::cin >> u >> v;
        g.addEdge(u, v);
    }

    g.printSCCs();

    return 0;
}
