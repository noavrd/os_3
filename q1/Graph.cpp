#include "Graph.hpp"
#include <iostream>
#include <algorithm>

Graph::Graph(int vertex) : vertex(vertex), adj(vertex + 1), rev_adj(vertex + 1) {}

void Graph::addEdge(int u, int v) {
    adj[u].push_back(v);
    rev_adj[v].push_back(u);
}

void Graph::fillOrder(int v, std::vector<bool>& visited, std::stack<int>& Stack) {
    visited[v] = true;
    for (int i : adj[v]) {
        if (!visited[i])
            fillOrder(i, visited, Stack);
    }
    Stack.push(v);
}

void Graph::dfs(int v, std::vector<bool>& visited, std::vector<int>& component) {
    visited[v] = true;
    component.push_back(v);
    for (int i : rev_adj[v]) {
        if (!visited[i])
            dfs(i, visited, component);
    }
}

void Graph::printSCCs() {
    std::stack<int> Stack;
    std::vector<bool> visited(vertex + 1, false);

    // Fill vertices in stack according to their finishing times
    for (int i = 1; i <= vertex; i++) {
        if (!visited[i])
            fillOrder(i, visited, Stack);
    }

    // Process all vertices in order defined by Stack
    std::fill(visited.begin(), visited.end(), false);

    while (!Stack.empty()) {
        int v = Stack.top();
        Stack.pop();

        if (!visited[v]) {
            std::vector<int> component;
            dfs(v, visited, component);
            
            // Output the component as an SCC
            for (int i : component) {
                std::cout << i << " ";
            }
            std::cout << std::endl;
        }
    }
}
