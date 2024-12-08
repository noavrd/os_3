#include "reactor.hpp"
#include "deque_graph.cpp"
#include <iostream>
#include <sstream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 9034

void handleClient(int fd, DequeGraph& graph) {
    char buffer[1024];

    while (true) {
        ssize_t bytesRead = read(fd, buffer, sizeof(buffer) - 1);
        if (bytesRead <= 0) {
            std::cout << "Client disconnected: " << fd << std::endl;
            close(fd);
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
            graph = DequeGraph(n);
            for (int i = 0; i < m; ++i) {
                int u, v;
                input >> u >> v;
                graph.addEdge(u, v);
            }
            std::cout << "Graph created with " << n << " vertices and " << m << " edges." << std::endl;
            send(fd, "Graph created.\n", 15, 0);
        } else if (cmd == "Kosaraju") {
            std::ostringstream result;
            graph.runKosaraju();  // This prints to stdout; you can redirect it to the result stream if needed
            std::cout << "Kosaraju algorithm executed for client: " << fd << std::endl;
        } else if (cmd == "Newedge") {
            int u, v;
            input >> u >> v;
            graph.addEdge(u, v);
            std::cout << "Edge added: " << u << " -> " << v << std::endl;
            send(fd, "Edge added.\n", 12, 0);
        } else if (cmd == "Removeedge") {
            int u, v;
            input >> u >> v;
            graph.removeEdge(u, v);
            std::cout << "Edge removed: " << u << " -> " << v << std::endl;
            send(fd, "Edge removed.\n", 14, 0);
        } else {
            std::cout << "Unknown command from client: " << fd << std::endl;
            send(fd, "Unknown command.\n", 17, 0);
        }
    }
}

int main() {
    Reactor reactor;
    DequeGraph graph(0);

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

    reactor.addFd(listener, [&](int fd) {
        int client = accept(fd, nullptr, nullptr);
        if (client < 0) {
            perror("Accept failed");
            return;
        }
        std::cout << "New client connected: " << client << std::endl;
        reactor.addFd(client, [&](int clientFd) {
            handleClient(clientFd, graph);
        });
    });

    reactor.start();
    close(listener);

    return 0;
}
