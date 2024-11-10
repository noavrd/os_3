// server.cpp
#include "deque_graph.cpp"
#include <iostream>
#include <sstream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT "9034"  // the port clients will be connecting to

// Function to handle client commands
void handleClientCommand(int client_fd, DequeGraph& graph, const std::string& command) {
    std::istringstream input(command);
    std::string cmd;
    input >> cmd;

    if (cmd == "Newgraph") {
        int n, m;
        input >> n >> m;
        graph = DequeGraph(n);  // Reinitialize graph

        for (int i = 0; i < m; i++) {
            std::string edgeLine;
            std::getline(std::cin, edgeLine);
            std::istringstream edgeStream(edgeLine);
            int u, v;
            edgeStream >> u >> v;
            graph.addEdge(u, v);
        }
        std::string response = "Graph created with " + std::to_string(n) + " vertices and " + std::to_string(m) + " edges.\n";
        send(client_fd, response.c_str(), response.length(), 0);

    } else if (cmd == "Kosaraju") {
        std::ostringstream resultStream;
        graph.runKosaraju();  // Run the algorithm and output to stdout (can modify to store in resultStream)
        std::string result = resultStream.str();
        send(client_fd, result.c_str(), result.length(), 0);

    } else if (cmd == "Newedge") {
        int u, v;
        input >> u >> v;
        graph.addEdge(u, v);
        std::string response = "Edge added from " + std::to_string(u) + " to " + std::to_string(v) + ".\n";
        send(client_fd, response.c_str(), response.length(), 0);

    } else if (cmd == "Removeedge") {
        int u, v;
        input >> u >> v;
        graph.removeEdge(u, v);
        std::string response = "Edge removed from " + std::to_string(u) + " to " + std::to_string(v) + ".\n";
        send(client_fd, response.c_str(), response.length(), 0);
    }
}

int main() {
    int listener; // Listening socket descriptor
    struct sockaddr_in server_addr;
    
    // Create a new graph shared across clients
    DequeGraph graph(0);

    // Set up socket
    listener = socket(AF_INET, SOCK_STREAM, 0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(9034);
    bind(listener, (struct sockaddr*)&server_addr, sizeof(server_addr));
    listen(listener, 10);

    std::cout << "Server listening on port 9034..." << std::endl;

    while (true) {
        struct sockaddr_storage client_addr;
        socklen_t addr_size = sizeof(client_addr);
        int client_fd = accept(listener, (struct sockaddr*)&client_addr, &addr_size);

        char buffer[1024];
        int bytes_received = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
        if (bytes_received > 0) {
            buffer[bytes_received] = '\0';
            handleClientCommand(client_fd, graph, std::string(buffer));
        }
        close(client_fd);
    }

    close(listener);
    return 0;
}
