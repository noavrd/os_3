
#ifndef GRAPH_HPP
#define GRAPH_HPP

class Graph {
public:
    virtual void addEdge(int u, int v) = 0;
    virtual void removeEdge(int u, int v) = 0;
    virtual void runKosaraju() = 0;
    virtual ~Graph() = default;
};

#endif 
