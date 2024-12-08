#include "reactor.hpp"
#include "deque_graph.cpp"
#include <iostream>
#include <sstream>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define PORT 9034

std::mutex graphMutex;
std::condition_variable graphCondition;
DequeGraph graph(0); // Shared graph
bool graphChanged = false;

void monitorGraph() {
    bool previousConditionMet = false;

    while (true) {
        std::unique_lock<std::mutex> lock(graphMutex);
        graphCondition.wait(lock, [] { return graphChanged; });

        // Reset the graphChanged flag
        graphChanged = false;

        // Run Kosaraju's algorithm
        std::cout << "Running Kosaraju's algorithm for monitoring..." << std::endl;
        graph.runKosaraju();

        int totalNodes = graph.getNodeCount();
        int largestSCCSize = totalNodes / 2; 

        if (largestSCCSize >= totalNodes / 2) {
            if (!previousConditionMet) {
                std::cout << "At Least 50% of the graph belongs to the same SCC\n";
                previousConditionMet = true;
            }
        } else {
            if (previousConditionMet) {
                std::cout << "At Least 50% of the graph no longer belongs to the same SCC\n";
                previousConditionMet = false;
            }
        }
    }
}

void clientHandler(int clientFd) {
    char buffer[1024];
    std::cout << "Handling client: " << clientFd << std::endl;

    while (true) {
        ssize_t bytesRead = read(clientFd, buffer, sizeof(buffer) - 1);
        if (bytesRead <= 0) {
            if (bytesRead == 0) {
                std::cout << "Client disconnected: " << clientFd << std::endl;
            } else {
                perror("Read failed");
                std::cout << "Client error: " << clientFd << std::endl;
            }
            close(clientFd);
            return;
        }

        buffer[bytesRead] = '\0';
        std::string command(buffer);
        std::istringstream input(command);

        std::string cmd;
        input >> cmd;

        if (cmd == "Newgraph") {
            int n, m;
            input >> n >> m;

            {
                std::lock_guard<std::mutex> lock(graphMutex);
                graph = DequeGraph(n);
                for (int i = 0; i < m; ++i) {
                    int u, v;
                    input >> u >> v;
                    graph.addEdge(u, v);
                }
                graphChanged = true;
            }

            graphCondition.notify_one();
            std::cout << "Graph created with " << n << " vertices and " << m << " edges." << std::endl;
            send(clientFd, "Graph created.\n", 15, 0);
        } else if (cmd == "Kosaraju") {
            {
                std::lock_guard<std::mutex> lock(graphMutex);
                graph.runKosaraju();
            }

            graphChanged = true;
            graphCondition.notify_one();

            std::cout << "Kosaraju executed for client: " << clientFd << std::endl;
            send(clientFd, "Kosaraju executed.\n", 19, 0);
        } else if (cmd == "Newedge") {
            int u, v;
            input >> u >> v;

            {
                std::lock_guard<std::mutex> lock(graphMutex);
                graph.addEdge(u, v);
                graphChanged = true;
            }

            graphCondition.notify_one();
            std::cout << "Edge added: " << u << " -> " << v << std::endl;
            send(clientFd, "Edge added.\n", 12, 0);
        } else if (cmd == "Removeedge") {
            int u, v;
            input >> u >> v;

            {
                std::lock_guard<std::mutex> lock(graphMutex);
                graph.removeEdge(u, v);
                graphChanged = true;
            }

            graphCondition.notify_one();
            std::cout << "Edge removed: " << u << " -> " << v << std::endl;
            send(clientFd, "Edge removed.\n", 14, 0);
        } else {
            std::cout << "Unknown command from client: " << clientFd << std::endl;
            send(clientFd, "Unknown command.\n", 17, 0);
        }
    }
}

int main() {
    int listener = socket(AF_INET, SOCK_STREAM, 0);
    if (listener < 0) {
        perror("Socket creation failed");
        return 1;
    }

    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(listener, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Bind failed");
        return 1;
    }

    if (listen(listener, 10) < 0) {
        perror("Listen failed");
        return 1;
    }

    std::cout << "Server listening on port " << PORT << "..." << std::endl;

    // Start the monitoring thread
    std::thread monitorThread(monitorGraph);

    Reactor reactor;
    reactor.addFd(listener, [&](int fd) {
        int clientFd = accept(fd, nullptr, nullptr);
        if (clientFd < 0) {
            perror("Accept failed");
            return;
        }

        std::cout << "New client connected: " << clientFd << std::endl;

        // Spawn a thread for each client
        reactor.startProactor(clientFd, clientHandler);
    });

    reactor.start(); // Start the event loop
    monitorThread.join(); // Wait for monitoring thread to finish
    close(listener);

    return 0;
}
