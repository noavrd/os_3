#include "deque_graph.cpp"
#include <iostream>
#include <sstream>
#include <thread>
#include <mutex>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 9034

std::mutex graphMutex;

void handleClient(int clientFd, DequeGraph& graph) {
    char buffer[1024];

    while (true) {
        ssize_t bytesRead = read(clientFd, buffer, sizeof(buffer) - 1);
        if (bytesRead <= 0) {
            std::cout << "Client disconnected: " << clientFd << std::endl;
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
            }

            std::cout << "Graph created with " << n << " vertices and " << m << " edges." << std::endl;
            send(clientFd, "Graph created.\n", 15, 0);
        } else if (cmd == "Kosaraju") {
            std::ostringstream result;

            {
                std::lock_guard<std::mutex> lock(graphMutex);
                graph.runKosaraju();  // Output results to stdout or capture them in the stream
            }

            std::cout << "Kosaraju algorithm executed for client: " << clientFd << std::endl;
            send(clientFd, "Kosaraju executed.\n", 19, 0);
        } else if (cmd == "Newedge") {
            int u, v;
            input >> u >> v;

            {
                std::lock_guard<std::mutex> lock(graphMutex);
                graph.addEdge(u, v);
            }

            std::cout << "Edge added: " << u << " -> " << v << std::endl;
            send(clientFd, "Edge added.\n", 12, 0);
        } else if (cmd == "Removeedge") {
            int u, v;
            input >> u >> v;

            {
                std::lock_guard<std::mutex> lock(graphMutex);
                graph.removeEdge(u, v);
            }

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
    serverAddr.sin_family = AF_INET;              // Address family
    serverAddr.sin_port = htons(PORT);            // Port number (network byte order)
    serverAddr.sin_addr.s_addr = INADDR_ANY;      // Bind to any available address

    if (bind(listener, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Bind failed");
        return 1;
    }

    if (listen(listener, 10) < 0) {
        perror("Listen failed");
        return 1;
    }

    std::cout << "Server listening on port " << PORT << "..." << std::endl;

    DequeGraph graph(0); // Shared graph across threads
    std::vector<std::thread> threads; // Store client threads

    while (true) {
        int clientFd = accept(listener, nullptr, nullptr);
        if (clientFd < 0) {
            perror("Accept failed");
            continue;
        }

        std::cout << "New client connected: " << clientFd << std::endl;

        // Spawn a new thread to handle the client
        threads.emplace_back(std::thread(handleClient, clientFd, std::ref(graph)));
    }

    for (auto& thread : threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }

    close(listener);
    return 0;
}
