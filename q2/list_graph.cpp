// list_graph.cpp
#include "graph.hpp"
#include <iostream>
#include <list>
#include <vector>
#include <stack>
#include <algorithm>

class ListGraph : public Graph {
    int V;
    std::vector<std::list<int>> adj;
    std::vector<std::list<int>> rev_adj;

public:
    ListGraph(int V) : V(V), adj(V + 1), rev_adj(V + 1) {}

    void addEdge(int u, int v) override {
        adj[u].push_back(v);
        rev_adj[v].push_back(u);
    }

    void runKosaraju() override {
        std::stack<int> Stack;
        std::vector<bool> visited(V + 1, false);

        for (int i = 1; i <= V; i++)
            if (!visited[i])
                fillOrder(i, visited, Stack);

        std::fill(visited.begin(), visited.end(), false);

        while (!Stack.empty()) {
            int v = Stack.top();
            Stack.pop();

            if (!visited[v]) {
                std::vector<int> component;
                dfs(v, visited, component);
                for (int i : component) std::cout << i << " ";
                std::cout << std::endl;
            }
        }
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
