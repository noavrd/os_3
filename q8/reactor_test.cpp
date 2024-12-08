#include "reactor.hpp"
#include <iostream>
#include <unistd.h>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>

void clientHandler(int clientFd) {
    char buffer[1024];
    while (true) {
        ssize_t bytesRead = read(clientFd, buffer, sizeof(buffer) - 1);
        if (bytesRead <= 0) {
            std::cout << "Client disconnected: " << clientFd << std::endl;
            return;
        }
        buffer[bytesRead] = '\0';
        std::cout << "Received: " << buffer << std::endl;
        send(clientFd, "Echo: ", 6, 0);
        send(clientFd, buffer, bytesRead, 0);
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
    serverAddr.sin_port = htons(9034);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(listener, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Bind failed");
        return 1;
    }

    if (listen(listener, 10) < 0) {
        perror("Listen failed");
        return 1;
    }

    std::cout << "Server listening on port 9034..." << std::endl;

    Reactor reactor;
    reactor.addFd(listener, [&](int fd) {
        int clientFd = accept(fd, nullptr, nullptr);
        if (clientFd < 0) {
            perror("Accept failed");
            return;
        }

        std::cout << "New client connected: " << clientFd << std::endl;

        // Start a Proactor thread for the client
        reactor.startProactor(clientFd, clientHandler);
    });

    reactor.start();
    return 0;
}
