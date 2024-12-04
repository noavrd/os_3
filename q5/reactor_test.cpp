#include "reactor.hpp"
#include <iostream>
#include <unistd.h>
#include <fcntl.h>

void handleInput(int fd) {
    char buffer[1024];
    ssize_t bytesRead = read(fd, buffer, sizeof(buffer) - 1);
    if (bytesRead > 0) {
        buffer[bytesRead] = '\0';
        std::cout << "Input received: " << buffer << std::endl;
    }
}

int main() {
    Reactor reactor;

    // Add STDIN to the reactor for testing
    int stdin_fd = fileno(stdin);
    if (stdin_fd < 0) {
        std::cerr << "Error getting stdin file descriptor\n";
        return 1;
    }

    reactor.addFd(stdin_fd, handleInput);

    std::cout << "Reactor test running. Type something and press Enter to test.\n";
    reactor.start();

    return 0;
}
