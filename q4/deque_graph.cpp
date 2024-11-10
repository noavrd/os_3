// deque_graph.cpp
#include "graph.hpp"
#include <iostream>
#include <deque>
#include <vector>
#include <stack>
#include <algorithm>
#include <sstream>

class DequeGraph : public Graph {
    int V;
    std::vector<std::deque<int>> adj;
    std::vector<std::deque<int>> rev_adj;

public:
    DequeGraph(int V) : V(V), adj(V + 1), rev_adj(V + 1) {}

    void addEdge(int u, int v) override {
        adj[u].push_back(v);
        rev_adj[v].push_back(u);
    }

    void removeEdge(int u, int v) override {
        adj[u].erase(std::remove(adj[u].begin(), adj[u].end(), v), adj[u].end());
        rev_adj[v].erase(std::remove(rev_adj[v].begin(), rev_adj[v].end(), u), rev_adj[v].end());
    }

    void runKosaraju() override {
        std::stack<int> Stack;
        std::vector<bool> visited(V + 1, false);

        for (int i = 1; i <= V; i++)
            if (!visited[i])
                fillOrder(i, visited, Stack);

        std::fill(visited.begin(), visited.end(), false);

        std::stringstream output;
        while (!Stack.empty()) {
            int v = Stack.top();
            Stack.pop();

            if (!visited[v]) {
                std::vector<int> component;
                dfs(v, visited, component);
                for (int i : component) output << i << " ";
                output << "\n";
            }
        }

        std::cout << output.str();  // Output Kosaraju result to stdout (for debugging/logging)
    }

private:
    void fillOrder(int v, std::vector<bool>& visited, std::stack<int>& Stack) {
        visited[v] = true;
        for (int i : adj[v])
            if (!visited[i])
                fillOrder(i, visited, Stack);
        Stack.push(v);
    }

    void dfs(int v, std::vector<bool>& visited, std::vector<int>& component) {
        visited[v] = true;
        component.push_back(v);
        for (int i : rev_adj[v])
            if (!visited[i])
                dfs(i, visited, component);
    }
};
